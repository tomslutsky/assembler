$(shell mkdir -p bin)

CREATOR = gcc -Wall -ansi -pedantic

assembler: bin/assembler.o bin/firstpass.o bin/secondpass.o bin/structs.o bin/parsing.o
	$(CREATOR) -g ./bin/assembler.o ./bin/firstpass.o ./bin/secondpass.o ./bin/structs.o ./bin/parsing.o -o assembler
bin/assembler.o: src/assembler.c
	$(CREATOR) -c src/assembler.c -o bin/assembler.o
bin/firstpass.o: src/firstpass.c
	$(CREATOR) -c src/firstpass.c -o bin/firstpass.o
bin/secondpass.o: src/secondpass.c
	$(CREATOR) -c src/secondpass.c -o bin/secondpass.o
bin/structs.o: src/structs.c
	$(CREATOR) -c src/structs.c -o bin/structs.o
bin/parsing.o: src/parsing.c
	$(CREATOR) -c src/parsing.c -o bin/parsing.o
