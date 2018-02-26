;This is a test program written in assembly code 



STR:		.string "This is my program"
ARRAY1:		.data 1,2,3,4,5
		.extern EX1
		.entry EN2
; comment line -------------------------------

NUM1:		.data 400
		cmp #500, NUM1

EN2:		sub EN1, EX1
		mov #210, r3[r6]
		dec r3[r6]
EN1:		.string "this is an entry"
; second comment line -----------------------

		.extern EX2
		.entry EN1
		.data 10, 20, 30, 40, 50
COMMAND:	jmp STR
		add r1,NUM1
SENTENCE:	.string "John Doe"
		lea EX2 EN2
		prn SENTENCE
		clr r2
		bne EX1
		stop
