#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MEM_START 100
#define MEM_SIZE 1000
#define MAX_LINE 80
#define MAX_NAME 200
#define HASHSIZE 101
#define LABEL_SIZE 30

#define UNUSED 7

#define DATA 0
#define INSTRUCTION 1
#define EXTERN 2
#define ENTRY 3

#define NUM_OF_TYPES 4
#define NUM_OF_OPCODES 16
#define NUM_OF_REGS 8

#define IMMEDIATE 0
#define DIRECT 1
#define INDEX 2
#define REGISTER 3

#define MOV 0
#define CMP 1
#define ADD 2
#define SUB 3
#define NOT 4
#define CLR 5
#define LEA 6
#define INC 7
#define DEC 8
#define JMP 9
#define BNE 10
#define RED 11
#define PRN 12
#define JSR 13
#define RTS 14
#define STOP 15

#define ABSOLUTE 0
#define EXTERNAL 1
#define RELOCATABLE 2

#define ONE 1	/*firstpass*/
#define TWO 2	/*secondpass*/

/* data sizes */
#define MAX_DATA 16383
#define MIN_DATA -16384
#define MAX_IMMEDIATE 4095
#define MIN_IMMEDIATE -4096

/*errors macros */
#define ERROR(line, msg) if(!error_flag){fprintf(stderr, "%s: error: line %d: %s\n",filename, line, msg); error_flag = 1; ++EC;}
#define WARN(line,msg) fprintf(stderr, "%s: warning: line %d: %s\n",filename, line, msg)
#define GENERR(msg) fprintf(stderr, "%s: %s\n", filename, msg)

#define MASK 0x7FFF /*for proper hexadicamel representation of negative numbers*/

/*    global variables    */
int IC,DC; /*  IC - instructions counter.  DC - date counter*/
int EC, LC; /* EC- errors counter.  LC - source line counter */
int label_flag;
int error_flag;

char filename[MAX_NAME]; /* for error messages */

/*   function declarations    */
int assemble(char *name);
int secondPass(char *s);
void add_ext(char *source, char *dest, char *ext);
int isIgnoreLine(char *s);
int readLine(char *s);
char *skipBlank(char *s);
char *nextField(char *s);
void getLabel(char *label,char *s);
int isVar(char *s);
int isLabel(char *s);
int isData(char *s);
int isExtern(char *s, int pass);
int isEntry(char *s);
int isCode(char *s);
int checkOps(int opCode, int opSource, int opDest);
void getField(char *field, char *s);

int getOpMethod(char *s);
int isImmidiate(char *s);
int isDirect(char *s);
int isIndex(char *s);
int isRegister(char *s);
int getGroup(int opCode);
int getRegister(char *s);

int install_label(char *name, int value, int type);
unsigned int hash(char *s);
int install_data(char *s);
int isNum(char *s);

void update_table();
int isDataLine(char *s);
int install_entry(char *line, char *en_name, FILE *en_file);
int write_file(FILE *ob_file, char *ob_name, int ins, int dat);
int install_operands(char *s, FILE *ex_file, char *ex_name);
int install_extern(char *s, FILE *ex_file, char *ex_name);
void free_lab_tab();
void initiate();
void print_table();

#include "structs.h"
#endif
