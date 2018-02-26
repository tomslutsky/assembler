#include "utils.h"


/* assembler program - main function -
	gets files names to assemble from command line and create machine code files */

int main(int argc, char *argv[]){
	int i,err=0;
	if(argc < 2){
		GENERR("need input files");
		exit(1);
	}
	/*loop iterating through files to be assembled */
	for(i=1;i<argc;i++){
		initiate();
		if(assemble(argv[i])==0)/*call the assemble routine */	
			printf("file: %s.as assembled successfuly\n", argv[i]);
		else {
			printf("file: %s.as errors were found. unable to assemble\n", argv[i]);
			err++;
		}
		free_lab_tab();	/* free memory allocated for assembling file */
	}
	return err;
}
