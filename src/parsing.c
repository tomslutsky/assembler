/************************************************************************************************************/
/*********************      parsing.c -                		     ****************************************/
/*********************   hold routines to parse ane analyze the text ****************************************/
/************************************************************************************************************/


#include "utils.h"
extern const char *opCodes[];
extern const char *registers[];
extern const char *types[];
extern char filename[];

/*assign label from s to label */
void getLabel(char *label, char *s){
	char *temp = label;
	while(*s!=':')
		*temp++ = *s++;
	*temp = '\0';
}

/*returns the number of the register listed in s */
int getRegister(char *s) {
	int i, reg=NUM_OF_REGS;
	for(i=0; i<NUM_OF_REGS;i++){
		if(!strncmp(s, registers[i],2))
			reg = i;
	}
	return reg;

}

/*get group code based on op code*/
int getGroup(int opCode){
	switch(opCode){
		case 0: return 2;
		case 1: return 2;
		case 2: return 2;
		case 3: return 2;
		case 4: return 1;
		case 5: return 1;
		case 6: return 2;
		case 7: return 1;
		case 8: return 1;
		case 9: return 1;
		case 10: return 1;
		case 11: return 1;
		case 12: return 1;
		case 13: return 1;
		case 14: return 0;
		case 15: return 0;
		default: return NUM_OF_OPCODES;
	}
}




/*returns word from text without white spaces */
void getField(char *field, char *s){
	while(!isspace(*s) && *s != ',')
		*field++ = *s++;
	*field = '\0';
}


/*returns pointer to the next non white space character of s*/
char *skipBlank(char *s){
	char *temp = s;
	while(isspace(*temp))
		++temp;
	return temp;
}

/*returns pointer to the next field of the input line s*/
char *nextField(char *s){
	s = skipBlank(s);
	while(!isspace(*s) && *s != ',')
		++s;
	++s;
	return s;
}

/*add extenstion to end of source string and assign in to dest string */
void add_ext(char *source, char *dest, char *ext){
	strcpy(dest,source);
	strcat(dest,ext);
}


/* if line is empty or comment return 1 . return 0 otherwise */
int isIgnoreLine(char *s){
	char *temp = s;
	if(*temp == ';')
		return 1;
	temp = skipBlank(temp);
	if(*temp == '\0')
		return 1;
	return 0;
}
	
/*returns 1 if s is a legal label name and 0 if not */
int isVar(char *s){
	char label[LABEL_SIZE];
	int i=0;
	strcpy(label,s);
	if(!isalpha(label[i])){
		ERROR(LC, "Illegal label's name");
		return 0;
	}
	for(i=0;i<NUM_OF_REGS;i++){
		if(strcmp(label,registers[i])==0){
			ERROR(LC, "Label's name cannot be a register");
			return 0;
		}
	}
	
	for(i=0;i<NUM_OF_OPCODES;i++){
		if(strcmp(label,opCodes[i])==0){
			ERROR(LC, "Label's name cannot be an instruction");
			return 0;
		}
	}
	return 1;

}

int getOpMethod(char *s){ /*returns the numeral vaule of the operand method */
	if(isImmidiate(s))
		return IMMEDIATE;
	if(isIndex(s))
		return INDEX;
	if(isRegister(s)){
		return REGISTER;}
	if(isDirect(s))
		return DIRECT;

	return NUM_OF_TYPES;
}



/*checks if line's operands structure is legal and returns number of lines to be saved in memory , 0 if error*/
int checkOps(int opCode, int opSource, int opDest){ 
	int lines = 1;
	if(opCode == STOP || opCode == RTS)
		return 1;
	if(opCode == JSR || opCode == RED || opCode == BNE || opCode == JMP || opCode == DEC || opCode == INC || opCode == CLR || opCode == NOT){
		if(opSource != NUM_OF_TYPES)
			return 0;
		if(opDest <1 || opDest >3)
			return 0;
	}
	if(opCode == PRN){
		if(opSource != NUM_OF_TYPES)
			return 0;
		if(opDest < 0 || opDest >3)
			return 0;
	}
	if(opCode == MOV || opCode == ADD || opCode == SUB){
		if(opSource < 0 || opSource > 3)
			return 0;
		if(opDest <1 || opDest >3)
			return 0;
	}
	if(opCode == CMP){
		if(opSource < 0 || opSource >3)
			return 0;
		if(opDest <0 || opDest > 3)
			return 0;
	}
	
	if(opCode == LEA){
		if(opSource < 1 || opSource > 2)
			return 0;
		if(opDest < 1 || opDest > 3)
			return 0;
	}

	if(opSource == 3 && opDest == 3)
		return 2;

	if(opSource != NUM_OF_TYPES)
		lines++;
	if(opDest != NUM_OF_TYPES)
		lines++;

	return lines;
}





int isNum(char *s){ /*check whether a token is a number with possible additional white spaces */
	int count=0;
	char *c = skipBlank(s);
	if(*c == '-')
	c++;
	while(isdigit(*c) != 0){
		++count;
		++c;
	}
	c = skipBlank(c);
	if(*c != '\0' || count ==0)
		return 0;
	return 1;
	
}

