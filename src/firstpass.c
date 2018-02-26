/*this is the first pass of the assemler.
here the symbols, entries and externals tables will be issued.
In addition the memory image of the commands and dats will be placed*/
#include "utils.h"

const char *types[] = {"data","string","entry","extern"};

const char *opCodes[] = {"mov","cmp", "add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};

const char *registers[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};


int assemble(char *name){ /*first pass main routine*/
	FILE *as_file;
	char line[MAX_LINE];
	char as_name[MAX_NAME];
	add_ext(name, as_name, ".as");
	strcpy(filename, as_name);
	
	as_file = fopen(as_name, "r");
	
	if(as_file == NULL){
		GENERR("cannot open source file");
		return 1;
	}

	/*initiate counters*/
	error_flag = 0;
	EC = 0;  /*error counter  */
	LC = 0;  /*source code line */
	IC = 0;
	DC = 0;
	
	
	
	while(fgets(line,MAX_LINE+1,as_file)!=NULL) { 
	/*  loop reading file's lines */
		
		++LC;
		error_flag = 0;
		if(isIgnoreLine(line)){ /* skip blank and comment lines */
			continue;
		}
		if(strlen(line) >= MAX_LINE -1) {  /* check if line is too long */
			ERROR(LC, "line it too long");
			continue;
		}
			
		readLine(line);
	} /*end of loop */
	
	fclose(as_file);

	/* check if program is too big for memory */
	if(IC + DC > MEM_SIZE){
		GENERR("insufficient RAM to run the program");
		return 1;
	}
	if(EC == 0){
		EC = secondPass(name);
	} 
	return EC;
}


/* subroutines  */


/*   readline    */
/*analyze current line from .as file.
checks for labels, instructions and commands */

int readLine(char *line){ 
	
	label_flag = 0;

	/* skip comments and blank line */
	if(*line == ';')
		return 1;
	line = skipBlank(line);
	if(*line == '\0')
		return 1;


	if(*line != '.' && isalpha(*line)==0){
		ERROR(LC, "Illegal start of expression");
		return 0;
	}

	if(isLabel(line)==1){ /* label was found and flagged */
		label_flag = 1;
	}

	if(isData(line)==1)  /* .data or .string were found and stored in memory */
		return 1;
	

	if(isExtern(line,ONE)==1) /* .extern was found and added to label's table */
		return 1;

	if(isEntry(line)==1) /* .entry was found. will be handled in second pass */
		return 1;
	
	if(isCode(line)==1) /* instruction code was found and stoerd in memory */
		return 1;
	
	return 0;
	
}



/*checks if a line contains a legal label. return 1 if true and 0 if not */
int isLabel(char *s) {
	char *temp = s;
	char label[LABEL_SIZE];
	int length=0;
	if(!isalpha(*temp))
		return 0;
	temp++;
	while(isalnum(*temp)){
		temp++;
		length++;
	}
	if(length>LABEL_SIZE){
		ERROR(LC, "Label is too long");
		return 0;
	}
	if(*temp == ':'){
		getLabel(label,s);
		if(!isVar(label))
			return 0;
	} else
		return 0;
	return 1;
}


/*checks if line is a .data or .string line. if true - stores the data and returns 1. if not returns 0 */
int isData(char *s) {
	char *temp = NULL;
	char *d, *st;
	char label[LABEL_SIZE];
	if((d = strstr(s, ".data")) == NULL && (st = strstr(s,".string")) == NULL)
		return 0; /* if .string and .data not found in line */
	temp = s;
	if(label_flag == 1){
		getLabel(label, temp);
		temp = nextField(temp);
		temp = skipBlank(temp);		
		if(*temp != '.'){
			ERROR(LC, ".data or .string misplaced");
			return 0;
		}

		install_label(label, DC, DATA); /*install label in labels table*/
	}
	return install_data(temp); /*return 1 if data installed successfuly. 0 otherwise.*/
}

/* checks if line is .extern line. returns 1 and store data if true. returns 0 if not */
int isExtern(char *s, int pass){
	char ex[LABEL_SIZE];
	char *c = NULL;
	if(!strstr(s, ".extern"))
		return 0; /* no .extern found */
	if(label_flag){
		WARN(LC, "Meaningless label");
		s = nextField(s);
		s = skipBlank(s);
	}
	s = nextField(s);
	s = skipBlank(s);
	getField(ex,s);
	c = ex;
	while(isalnum(*c))
		c++;
	*c = '\0';
	if(!isVar(ex))
		return 0;
	if(pass == ONE)
		return install_label(ex, 0, EXTERN);
	return 1;
}


/* checks if a line is a legal .entry line. if true returns 1. if not returns 0 */
int isEntry(char *s){
	char en[LABEL_SIZE];
	nlist *temp = NULL;
	if(!strstr(s, ".entry"))
		return 0; /* no .entry found */
	if(label_flag){
		WARN(LC, "Meaningless label.");
		s = nextField(s);
		s = skipBlank(s);
	}
	s = nextField(s);
	s = skipBlank(s);
	getField(en,s);
	if(!isVar(en))
		return 0;
	temp = look_for_label(en);
	if(temp){
		if(temp->type == EXTERN){
			ERROR(LC, "Label is already extern, can't be an entry");
			return 0;
		}
	}
	return 1;
}


/*checks if line is a legal instruction line. if true - return 1 stores in memory and keep memory for second pass if needed. if not returns 0 */
int isCode(char *s){
	int i,lines, opCode=NUM_OF_OPCODES, opSource=NUM_OF_TYPES, opDest=NUM_OF_TYPES, group, era=0;
	char field[LABEL_SIZE], label[LABEL_SIZE];
	char *temp = NULL;
	if(label_flag){ /*find and install label*/
		getLabel(label,s);
		install_label(label,IC,INSTRUCTION);
		temp = nextField(s);
	} else
		temp = s;
	temp = skipBlank(temp);
	getField(field,temp);

	for(i=0;i<NUM_OF_OPCODES;i++){
		if(strcmp(opCodes[i], field)==0){
			opCode = i;
			break;
		}
	}
	if(opCode == NUM_OF_OPCODES){
		/*illegal op code */
		ERROR(LC, "Illegal instruction");
		return 0;
	}
	
	temp = nextField(temp);
	temp = skipBlank(temp);
	group = getGroup(opCode);
	if(group == 2){
	/*collect first operand out of 2 */
		getField(field,temp);
		if((opSource = getOpMethod(field)) == NUM_OF_TYPES){
			ERROR(LC, "Unknown source operand");
			return 0;
		}
		
		temp = nextField(temp);
		temp = skipBlank(temp);

		if(*temp == ','){
			ERROR(LC, "Misplaced comma character");
			return 0;
		}
		if(*temp == '\0'){
			ERROR(LC, "Missing operand");
			return 0;
		}
		temp = skipBlank(temp);
		
	} /*end of group == 2*/

	if(group >0){
	/*collect second or only operand*/
		getField(field,temp);
		if((opDest = getOpMethod(field)) == NUM_OF_TYPES){
			ERROR(LC, "Unkown target operand");
			return 0;
		}
		temp = nextField(temp);
		temp = skipBlank(temp);

	}
	if(group == 0){
		temp = skipBlank(temp);
		if(*temp != '\0'){
			ERROR(LC, "Illegal characters at end of expression");
			return 0;
		}
	}
	if((lines = checkOps(opCode, opSource, opDest)) == 0){
		ERROR(LC, "Instruction does not match operands");
		return 0;
	}
	/*check for trailing characters*/
	if(*temp != '\0'){
		ERROR(LC, "Unknown characters in expression");
		return 0;	
	}
	
	/*adding word to memory image without following operands and updating IC */
	code_image[IC].word.unused = UNUSED;
	code_image[IC].word.group = getGroup(opCode);
	code_image[IC].word.opCode = opCode;
	code_image[IC].word.opSource = opSource;
	code_image[IC].word.opDest = opDest;
	code_image[IC].word.era = era;
	code_image[IC].word.pad = 0;
	IC += lines;
return 1;

}


int isImmidiate(char *s){ /*checks if s is an immediate number */
	char *op = s;
	char *temp = NULL;
	int sign=1,num=0;
	if(*op == '#'){
		op++;
		if(*op == '-'){
			op++;
			sign = -1;
		}
		temp = op;
		while(isdigit(*op))
			op++;
		op = skipBlank(op);
		if(*op != ',' && *op != '\0')
			return 0;
		while(isdigit(*temp)){
			num = num*10+ (*temp-'0');
			temp++;
		}
		num = num*sign;
		if(num < MIN_IMMEDIATE || num > MAX_IMMEDIATE){
			ERROR(LC, "Value is out of range for immediate value");
			return 0;
		}
		return 1;
	}
	return 0;
}

int isDirect(char *s){ /*checks if s is a direct reference to label */
	char *op = s;
	if(!(isalpha(*op)))
		return 0;
	while(isalnum(*op))
		op++;
	op = skipBlank(op);
	if(*op != ',' && *op != '\0')
		return 0;
	return 1;
}


int isIndex(char *s){ /*checks if s is call to memory by registers values */
	int i, reg=NUM_OF_REGS;
	char *op = s;
	for(i=1;i<NUM_OF_REGS;i+=2){
		if(!strncmp(op, registers[i],2))
			reg = i;
	}

	if(reg == NUM_OF_REGS)
		return 0;
	op+=2;
	if(*op != '[')
		return 0;
	op++;
	reg = NUM_OF_REGS;
	for(i=0;i<NUM_OF_REGS;i+=2){
		if(!strncmp(op, registers[i],2))
			reg = i;
	}
	if(reg == NUM_OF_REGS)
		return 0;
	op+=2;
	if(*op != ']')
		return 0;
	return 1;
}




int isRegister(char *s){ /*checks if s is a register name */
	char *temp = s;
	if(getRegister(temp)==NUM_OF_REGS)
		return 0;
	temp+=2;
	if(*temp != '\0' && *temp != ',' && !isspace(*temp))
		return 0;
	return 1;
}



