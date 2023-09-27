#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"
#include "Tree.h"

typedef struct{								// Parser object
	Array *tokens;							// Array of tokens
	Tree *tree;								// parse tree(root)
	Array *stack;
	int tokenIdx;							// index of tokens
}Parser;

Parser *parse(char *text);

Parser *ParserNew();
void ParserParse(Parser *p, char *text);
void ParserFree(Parser *parser);

char *nextToken(Parser *p);
char *tokenToType(char *token);
Tree *push(Parser *p, char* term);
Tree *pop(Parser *p, char* term);
Tree *parseProg(Parser *p);
void parseBlock(Parser *p);
void parseFor(Parser *p);
void parseBaseList(Parser *p);
void parseBase(Parser *p);
void parseStmt(Parser *p);
void parseExp(Parser *p);
void parseCond(Parser *p);
void error();
BOOL isEnd(Parser *p);
BOOL isNext(Parser *p, char *pTypes);
char *next(Parser *p, char *pTypes);

#endif
