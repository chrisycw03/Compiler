#include "Assembler.h"              
#include "Compiler.h"

#define TEST 1
#define C0C 2
#define AS0 3
#define VM0 4

void argError(char *msg) {								// condition of arguments error
  printf("%s\n", msg);                                                                        
  exit(1);                                                                                    
}

int main(int argc, char *argv[]) {						// Main Function
	char cFile0[] = "sum.c0", *cFile = cFile0;			// default program file
	char asmFile0[] = "test.asm0", *asmFile = asmFile0;	// default assembly file
	char objFile0[] = "test.obj0", *objFile = objFile0;	// default object file

#if TARGET==TEST										// compile TARGET==TEST
	ArrayTest();										// test Array Object
	HashTableTest();									// test HashTable Object
	OpTableTest();										// test OpTable Object
	compile(cFile, asmFile);							// test Compiler
	assemble(asmFile, objFile);							// test Assembler
	runObjFile(objFile);								// test Virtual Machine
	checkMemory();										// check memory usage

#elif TARGET==C0C										// compile TARGET==C0C, output compiler
	if (argc == 3) {
		cFile=argv[1]; asmFile=argv[2];					// set cFile and asmFile
	} else
    argError("c0c <c0File> <asmFile>");				// show argument error message
	compile(cFile, asmFile);

#elif TARGET==AS0										// compile TARGET==AS0, output assembler
	if (argc == 3) {
		asmFile=argv[1]; objFile=argv[2];				// set asmFile and objFile
	} else
		argError("as0 <asmFile> <objFile>");			// show argument error message
assemble(asmFile, objFile);

#elif TARGET==VM0										// compile TARGET==VM0, output virtual machine
	if (argc == 2)
		objFile=argv[1];								// set objFile
	else
		argError("vm0 <objFile>");						// show argument error message
	runObjFile(objFile);
#endif

	return 0;
}
