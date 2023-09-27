GCC=gcc
OBJ=Tree.o Scanner.o Parser.o OpTable.o Lib.o HashTable.o Generator.o cpu0.o Compiler.o Assembler.o Array.o
RM=rm -f

PHONY: $(OBJ) test c0c asm0 vm0

test: $(OBJ)
	$(GCC) main.c $(OBJ) -DTARGET=TEST -o test

c0c: $(OBJ)
	$(GCC) main.c $(OBJ) -DTARGET=C0C -o c0c

asm0: $(OBJ)
	$(GCC) main.c $(OBJ) -DTARGET=AS0 -o asm0

vm0: $(OBJ)
	$(GCC) main.c $(OBJ) -DTARGET=VM0 -o vm0

clean:
	$(RM) $(OBJ) test c0c asm0 vm0

Tree.o: Tree.c
	$(GCC) -c Tree.c -o Tree.o -g3

Scanner.o: Scanner.c
	$(GCC) -c Scanner.c -o Scanner.o -g3

Parser.o: Parser.c
	$(GCC) -c Parser.c -o Parser.o -g3
	
OpTable.o: OpTable.o
	$(GCC) -c OpTable.c -o OpTable.o -g3

Lib.o: Lib.c
	$(GCC) -c Lib.c -o Lib.o -g3

HashTable.o: HashTable.c
	$(GCC) -c HashTable.c -o HashTable.o -g3

Generator.o: Generator.c
	$(GCC) -c Generator.c -o Generator.o -g3

cpu0.o: cpu0.c
	$(GCC) -c cpu0.c -o cpu0.o -g3

Compiler.o: Compiler.c
	$(GCC) -c Compiler.c -o Compiler.o -g3

Assembler.o: Assembler.c
	$(GCC) -c Assembler.c -o Assembler.o -g3

Array.o: Array.c
	$(GCC) -c Array.c -o Array.o -g3