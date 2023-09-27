#ifndef SCANNER_H
#define SCANNER_H

#include "Array.h"

typedef struct{				// constructor of Scanner
	char *text;				// imported program text
	int textLen;			// program text length
	int textIdx;			// cuurent position of Scanner
	char token[MAX_LEN];	// current scanned tokens
}Scanner;

Scanner *ScannerNew(char *pText);
void ScannerFree(Scanner *scanner);
char *ScannerScan(Scanner *scanner);
Array *tokenize(char *text);
char *tokenToType(char *token);
void printTokens(Array *token);

extern char STRING[];
extern char NUMBER[];
extern char ID[];
extern char KEYWORDS[];
extern char OP1[];
extern char OP2[];
extern char COND_OP[];
extern char ITEM[];

#endif
