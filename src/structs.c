/* this file will contain the structures of labels, entries, externs and data/string tables */
#include "utils.h"

nlist *lab_tab[HASHSIZE];
binWord code_image[MEM_SIZE]; /*memory image of the instruction section */
binWord data_image[MEM_SIZE]; /*temporary memory image of data section */

extern char filename[];
/*    labels  - table was declared in utils.h  */

void initiate(){
	int i;
	for(i=0;i<HASHSIZE;i++)
		lab_tab[i] = NULL;
}

unsigned int hash(char *s) { /* hashes string s*/
	unsigned int hashval;
	for(hashval = 0; *s != '\0';s++) {
		hashval = *s + 31 * hashval;
	}
	return hashval % HASHSIZE;
}


nlist *look_for_label(char *s) { /*look for string s in table */
	nlist *temp;
	for(temp=lab_tab[hash(s)]; temp != NULL; temp = temp->next){
		if(strcmp(s,temp->name)==0)
			return temp;
	}
	return NULL;
}

void print_table() {
	nlist *temp = NULL;
	int i;
	for(i=0;i<HASHSIZE;i++){
		temp = lab_tab[i];
		while(temp){
			printf("%s  :  %X\n", temp->name, temp->value);
			temp = temp->next;
		}
	}
}




int install_label(char *name, int value, int type) { /*add name to label's table */
	nlist *temp = NULL;
	nlist *label = NULL;
	unsigned int hashval;
	label = look_for_label(name);	
	if(!label){ /*if name is not in table */
		temp = (nlist *)malloc(sizeof(nlist));
		if(temp == NULL){
			ERROR(LC, "Not enough available memory to allocate");
			return 0;
		}
		hashval = hash(name);
		temp->next = lab_tab[hashval];
		temp->type=type;
		temp->value = value;
		strcpy(temp->name,name);
		lab_tab[hashval] = temp;
	} else if(label->type == EXTERN)
		return 1;
	else {  /*label found in table */
		ERROR(LC, "Duplicate labels were found.");
		return 0;
	}
	return 1;
}

/* update_table() -
updates labels table to fit at the end of the code sections */
void update_table() {
	int i;
	nlist *temp = NULL;
	for(i=0;i<HASHSIZE;i++){
		temp = lab_tab[i];
		while(temp != NULL){
			if(temp->type == DATA)
				temp->value += IC;
			if(temp->type!=EXTERN)
				temp->value += MEM_START;
			temp = temp->next;
		}/*end while*/
	} /*end for*/
} /*end of update_table*/



void free_lab_tab() {
	nlist *temp = NULL;
	int i;
	for(i=0;i<HASHSIZE;i++){
		while(lab_tab[i]){
			temp = lab_tab[i];
			lab_tab[i] = lab_tab[i]->next;
			free(temp);
		}
		lab_tab[i]=NULL;
	}
}

/*    data/string    */

binWord data_image[MEM_SIZE]; /*memory image of the data section */





/*installs the data in s (both data and string) to data_image, returns 1 if succeeded and 0 if not*/
int install_data(char *s){
	int sign = 1; /*positive or negative number*/
	char *temp = s;
	char *check = NULL;
	int num;
	if(*temp != '.')
		return 0;
	temp++;
	/* numbers */
	if(strncmp(temp,"data ",4)==0){
		temp = nextField(temp);
		temp = skipBlank(temp);
		/*check of data consist of legal chars */
		check = temp;
		check = skipBlank(check);
			if(*check == '\0'){
				ERROR(LC, "numeral value missing");
				return 0;
		}
		while(*check != '\0'){
			if((!isdigit(*check)) && ((*check) != '-')  && ((*check) != '+') && ((*check) != ',') && (!isspace(*check))){
				ERROR(LC, "Illegal characters in data parameter");
				return 0;
			}
			
			if(*check == ','){
				check++;
				check = skipBlank(check);
			}
			if(*check == ','){
				ERROR(LC, "Numeric value missing");
				return 0;
			}
			check++;
		}

		while(*temp != '\0'){ /*loop colecting all numbers*/
			if(*temp == '+')
				temp++;
			if(!isdigit(*temp) && (*temp != '-')){
				ERROR(LC, "Illegal characters in data parameter");
				return 0;
			}
			num = 0;
			if(*temp == '-'){
				temp++;
				sign = -1;
			} else if(*temp == '+')
				temp++;
			else
				sign =1;
			while(isdigit(*temp)){ /*loop collecting digits and adding to number */
				if(*temp == '-'){
					ERROR(LC, "Illegal characters in data parameter");
					return 0;
				}
				num = num*10+ (*temp-'0');
				temp++;
			}
			num = num*sign;
			if(num < MIN_DATA || num > MAX_DATA){
				ERROR(LC, "Value is out of range for data variable");
				return 0;
			}
			data_image[DC++].num = num;
			temp = skipBlank(temp);

			if(*temp != ',' && *temp != '\0'){
				ERROR(LC, "Illegal characters in data parameter");
				return 0;
			}

			if(*temp == ','){
				temp++;
				temp = skipBlank(temp);
			}
			
		}
		return 1;	
	}
	
	/* strings */
	if(strncmp(temp,"string ",6)==0){
		temp=nextField(temp);
		temp = skipBlank(temp);
		if(*temp != '\"'){
			ERROR(LC, "Missing initiating string character");
			return 0;
		}
		temp++;
		while(*temp != '\"' && *temp != '\0'){
			data_image[DC++].num = *temp;
			temp++;
		}
		data_image[DC++].num = 0;
		if(*temp == '\0'){
			ERROR(LC, "Missing terminating string character");
			return 0;
		}
		temp++;
		temp=skipBlank(temp);
		if(*temp != '\0') {
			ERROR(LC, "Illegal characters at end of string expression");
			return 0;
		}
		return 1;
		
	}
	return 0;		
}



