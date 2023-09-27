#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Cpu0.h"

typedef struct {									// Object of Assembler
	Array *codes;									// Array of 'AsmCode'
	HashTable *symTable;							// Symbol Table
	HashTable *opTable;								// Operation Table
} Assembler; 

typedef struct { 									// Object of Assembly code
	int address;									// address of instruction
	int opCode;										// operation code of instruction
	int size;										// memory size of instruction
	char *label;									// label of instruction
	char *op;										// operators of instruction
	char *args;										// parameters of instruction
	char type;										// type of instruction
	char *objCode;									// Object Code of instruction
} AsmCode;

void assemble(char *asmFile, char *objFile);		// main function of Assembler

Assembler* AsmNew(); 								// constructor of Assembler
void AsmFree(Assembler *a);							// deconstructor of Assembler

void AsmPass1(Assembler *a, char *text);			// first stage of Assembling
void AsmPass2(Assembler *a);						// second stage of Assembling
void AsmSaveObjFile(Assembler *a, char *objFile);	// save Object Code
void AsmTranslateCode(Assembler *a, AsmCode *code);	// translate Assembly Code to Object Code

AsmCode* AsmCodeNew(char *line);					// constructor of Assembly Code
void AsmCodeFree(AsmCode *code);					// deconstructor of Assembly Code
void AsmCodePrintln(AsmCode *code);					// print AsmCode
int AsmCodeSize(AsmCode *code);						// calculate memory size of instruction

#endif
