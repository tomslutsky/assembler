#ifndef STRUCTS_H
#define STRUCTS_H




/*    structers     */


typedef union { /*represantation of a binary word */
	struct {
		unsigned int era: 2;
		unsigned int opDest: 2;
		unsigned int opSource: 2;
		unsigned int opCode: 4;
		unsigned int group: 2;
		unsigned int unused: 3;
		unsigned int pad: 1;

	} word;
	struct {
		unsigned int era: 2;
		unsigned int value: 13;
		unsigned int pad: 1;
	} direct;
	struct {
		unsigned int era: 2;
		unsigned int second_reg: 6;
		unsigned int first_reg: 6;
		unsigned int pad: 1;
	} regs;
	unsigned short num;
} binWord;



extern binWord code_image[MEM_SIZE]; /*memory image of the instruction section */
extern binWord data_image[MEM_SIZE]; /*temporary memory image of data section */

typedef struct node *ptr;
typedef struct node { /*item of linked list */
	char name[LABEL_SIZE];
	int value;
	int type;
	ptr next;
} nlist;


extern nlist *lab_tab[HASHSIZE]; /*lable's table*/


nlist *look_for_label(char *s);
#endif

