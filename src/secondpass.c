#include "utils.h"

/*second pass of the assembler program */
/***************************************/
/*build entire memory image and make .ob .en .ex files*/
/******************************************************/

extern const char *types[];

extern const char *opCodes[];

extern const char *registers[];

int entry_flag;
int extern_flag;
extern char filename[];

/*************************************************************************************************************/
/******************************    main routine of the second pass *******************************************/
/*************************************************************************************************************/

int secondPass(char *name){

	FILE *as_file=NULL, *en_file=NULL, *ex_file=NULL, *ob_file=NULL;
	char cur_line[MAX_LINE];
	char *line = NULL;
	char as_name[MAX_NAME], ob_name[MAX_NAME], en_name[MAX_NAME], ex_name[MAX_NAME];
	int ins_count = IC, dat_count = DC;

	/*creating file names with proper extensions */
	add_ext(name, as_name, ".as");
	add_ext(name, ob_name, ".ob");
	add_ext(name, en_name, ".ent");
	add_ext(name, ex_name, ".ext");

	/*initiating flags*/
	entry_flag = 0;
	extern_flag = 0;
	label_flag=0;
	as_file = fopen(as_name, "r");

	if(as_file == NULL){
		GENERR("Can't open source file");
		return 0;
	}

	
	/*update label's table with IC */
	update_table();

	/* initiate instructions counter */
	IC = 0;
	LC=0;
	/* loop - iterating through input's lines */
	while(fgets(cur_line,MAX_LINE,as_file)!=NULL) {
		++LC;
		line = cur_line;
		error_flag = 0;
		if(isIgnoreLine(line))
			continue;

		/*skip labels*/
		if(isLabel(line)){
			line = nextField(line);
			line = skipBlank(line);
		}

		/*skip all line if it's .data or .string */
		if(isDataLine(line))
			continue;

		/* handle entry lines */
		if(isEntry(line)){
			install_entry(line, en_name, en_file);
			continue;
		}

		/*handle extern lines */
		if(isExtern(line, TWO)){
			continue;
		}

		/* analyze line and add operands to memory_image */
		if(!install_operands(line, ex_file, ex_name))
			continue;
		
	}/*end loop*/

	fclose(as_file);
	
	/* create .ob file */
	if(EC == 0)
		write_file(ob_file,ob_name, ins_count, dat_count);
	else {
		remove(ex_name);
		remove(en_name);
	}
	
	return EC;
	
} /*end secondPass */


/********************************************************************/
/***************** sub routines *************************************/
/********************************************************************/



/* isDataLine - returns 1 if line is .data or .string. returns 0 if not */
int isDataLine(char *s){
	if(strstr(s,".data"))
		return 1;
	if(strstr(s,".string"))
		return 1;
	return 0;
}/*end of isDataLine*/


/* install_entry(char *s) - writes entry to file */
int install_entry(char *line, char *en_name, FILE *en_file){
	ptr temp = NULL;
	char label[MAX_NAME];
	if(entry_flag == 0){
		en_file = fopen(en_name, "w");
		entry_flag = 1;
	} else
		en_file = fopen(en_name, "a");
	if(!en_file){
		GENERR("Can't open entry file");
		return 0;
	}
	line = nextField(line);
	line = skipBlank(line);
	getField(label, line);
	temp = look_for_label(label);
	if(!temp){
		ERROR(LC, "Undefined entry");
		return 0;
	}
	fprintf(en_file, "%s	%X\n", temp->name, temp->value);
	fclose(en_file);
	return 1;
}/*end install_entry */


/* install_extern(char *s) - writes extern to file */
int install_extern(char *s, FILE *ex_file, char *ex_name){
	if(extern_flag == 0){
		ex_file = fopen(ex_name, "w");
		extern_flag = 1;
	} else
		ex_file = fopen(ex_name, "a");
	if(!ex_file){
		GENERR("Can't open extern file");
		return 0;
	}
	fprintf(ex_file, "%s	%X\n", s, IC+MEM_START);
	fclose(ex_file);
	return 1;
}/*end install_extern */


/* write_file() - writes memory image to .ob file */
int write_file(FILE *ob_file, char *ob_name, int ins, int dat){
	int i;
	ob_file = fopen(ob_name, "w");
	if(!ob_file){
		GENERR("Unable to create object file");
		return 0;
	}
	fprintf(ob_file, "%x	%X\n", ins, dat);
	for(i=0;i<IC;i++)
		fprintf(ob_file, "%X	%04X\n", i+MEM_START, ((code_image[i].num)&MASK));
	for(i=0;i<DC;i++)
		fprintf(ob_file, "%X	%04X\n", i+IC+MEM_START, ((data_image[i].num)&MASK));
	fclose(ob_file);
	return 1;
}/*end write_file*/

/* install_operands - add additional binary words to memory image */
int install_operands(char *line, FILE *ex_file, char *ex_name){
	int i, value, opSource=NUM_OF_TYPES, opDest=NUM_OF_TYPES, group, opCode = NUM_OF_OPCODES;
	ptr temp = NULL;
	char *s = line;
	char field[LABEL_SIZE];
	char source[LABEL_SIZE];
	char dest[LABEL_SIZE];
	IC++;
	s = skipBlank(s);
	getField(field,s);
	for(i=0;i<NUM_OF_OPCODES;i++){
		if(strcmp(opCodes[i], field)==0){
			opCode = i;
			break;
		}
	}
	group = getGroup(opCode);
	s = nextField(s);
	s = skipBlank(s);
	if(group == 2){
		getField(source,s);
		opSource = getOpMethod(source);
		s = nextField(s);
		s = skipBlank(s);
		if(*s == ','){
			s++;
			s = skipBlank(s);
		}
	}
	
	if(group >0){
		getField(dest,s);
		opDest = getOpMethod(dest);
	}


	if(opSource == IMMEDIATE){
		char *c = source;
		int sign = 1;
		value=0;
		c++;
		if(*c == '-'){
			sign = -1;
			c++;
		}
		while(isdigit(*c)){
			value = value*10+(*c-'0');
			c++;
		}
		code_image[IC].direct.value = value*sign;
		code_image[IC].direct.era = ABSOLUTE;
		IC++;
	}

	if(opSource == DIRECT){
		temp = look_for_label(source);
		if(!temp){
			ERROR(LC, "Undeclared label as source operand");
			return 0;
		}
		code_image[IC].direct.value = temp->value;
		code_image[IC].direct.era = RELOCATABLE;
		code_image[IC].direct.pad = 0;
		if(temp->type == EXTERN){
			code_image[IC].direct.era = EXTERNAL;
			install_extern(source, ex_file, ex_name);
		}
		IC++;	
	}

	if(opSource == INDEX){
		char *c = source;
		while(!isdigit(*c))
			c++;
		code_image[IC].regs.second_reg = *c - '0';
		c++;
		while(!isdigit(*c))
			c++;
		code_image[IC].regs.first_reg = *c - '0';
		code_image[IC].regs.era = ABSOLUTE;
		code_image[IC].regs.pad = 0;
		IC++;
	}
	
	if(opSource == REGISTER && opDest != REGISTER){
		char *c = source;
		while(!isdigit(*c))
			c++;
		code_image[IC].regs.first_reg = *c - '0';
		code_image[IC].regs.second_reg = 0;
		code_image[IC].regs.pad = 0;
		code_image[IC].regs.era = ABSOLUTE;
		IC++;
	}

	if(opDest == IMMEDIATE){
		char *c = dest;
		int sign = 1;
		value=0;
		c++;
		if(*c == '-'){
			sign = -1;
			c++;
		}
		while(isdigit(*c)){
			value = value*10+(*c-'0');
			c++;
		}
		value = value * sign;
		code_image[IC].direct.value = value;
		code_image[IC].direct.era = ABSOLUTE;
		IC++;
	}



	if(opDest == DIRECT){
		temp = look_for_label(dest);
		if(!temp){
			ERROR(LC, "Undeclared label as target operand");
			return 0;
		}
		code_image[IC].direct.value = temp->value;
		code_image[IC].direct.era = RELOCATABLE;
		code_image[IC].direct.pad = 0;
		if(temp->type == EXTERN){
			code_image[IC].direct.era = EXTERNAL;
			install_extern(dest, ex_file, ex_name);
		}
		IC++;	
	}



	if(opDest == INDEX){
		char *c = dest;
		while(!isdigit(*c))
			c++;
		code_image[IC].regs.second_reg = *c - '0';
		c++;
		while(!isdigit(*c))
			c++;
		code_image[IC].regs.first_reg = *c - '0';
		code_image[IC].regs.era = ABSOLUTE;
		code_image[IC].regs.pad = 0;
		IC++;
	}

	if(opDest == REGISTER && opSource != REGISTER){
		char *c = dest;
		while(!isdigit(*c))
			c++;
		code_image[IC].regs.second_reg = *c - '0';
		code_image[IC].regs.first_reg = 0;
		code_image[IC].regs.pad = 0;
		code_image[IC].regs.era = ABSOLUTE;
		IC++;
	}


	if(opSource == REGISTER && opDest == REGISTER){
		char *c = source;
		while(!isdigit(*c))
			c++;
		code_image[IC].regs.first_reg = *c - '0';
		c = dest;
		while(!isdigit(*c))
			c++;
		code_image[IC].regs.second_reg = *c - '0';
		code_image[IC].regs.era = ABSOLUTE;
		code_image[IC].regs.pad = 0;
		IC++;
	}


	return 1;
		
	
	
}/*end install_operands*/
