#include "Parser.h"

Parser *parse(char *text){							// main function of Parser
	Parser *p = ParserNew();						// construct a Parser
	ParserParse(p, text);							// parse the program text
	return p;
}

Parser *ParserNew(){								// constructor of Parser
	Parser *parser = ObjNew(Parser, 1);
	parser->tokens = NULL;
	parser->tree = NULL;
	parser->stack = ArrayNew(10);					// create an Array as a stack
	return parser;
}

void ParserFree(Parser *parser){
	ArrayFree(parser->tokens, strFree);
	ArrayFree(parser->stack, NULL);
	TreeFree(parser->tree);
	ObjFree(parser);
}

void ParserParse(Parser *p, char *text){			// function to parse	
	puts("===============tokenize==============="); 
	p->tokens = tokenize(text);						// scan text to Array
	printTokens(p->tokens);
	p->tokenIdx = 0;
	puts("===============parsing================");
	p->tree = parseProg(p);							// create syntax tree(root)
	if(p->stack->count != 0){						// parse fail if stack is not empty after parsing
		printf("parse fail:stack.count = %d\n", p->stack->count);
		error();
	}
}

void error(){
	puts("error()!");
	exit(1);
}

Tree *parseProg(Parser *p){							// PROG = BaseList
	push(p, "PROG");
	parseBaseList(p);
	return pop(p, "PROG");
}

void parseBaseList(Parser *p){						// BaseList = BASE
	push(p, "BaseList");
	while(!isEnd(p) && !isNext(p, "}"))
		parseBase(p);
	pop(p, "BaseList");
}

void parseBase(Parser *p){							// BASE = STMT | FOR
	push(p, "BASE");
	if(isNext(p, "for"))
		parseFor(p);
	else{
		parseStmt(p);
		next(p, ";");
	}
	pop(p, "BASE");
}

void parseFor(Parser *p){							// FOR = for(STMT ;;COND ; STMT) BLOCK
	push(p, "FOR");
	next(p, "for");
	next(p, "(");
	parseStmt(p);
	next(p, ";");
	parseCond(p);
	next(p, ";");
	parseStmt(p);
	next(p, ")");
	parseBlock(p);
	pop(p, "FOR");
}

void parseBlock(Parser *p){							// BLOCK = {BaseList}
	push(p, "BLOCK");
	next(p, "{");
	parseBaseList(p);
	next(p, "}");
	pop(p, "BLOCK");
}

void parseStmt(Parser *p){							// STMT = return id | id = EXP | id OP1
	push(p, "STMT");
	if(isNext(p, "return")){
		next(p, "return");
		next(p, "id");
	}else{
		next(p, "id");
		if(isNext(p, "=")){
			next(p, "=");
			parseExp(p);
		}else{
			next(p, OP1);
		}
	}
	pop(p, "STMT");
}

void parseExp(Parser *p){							// EXP = ITEM | ITEM [+-*/] ITEM
	push(p, "EXP");
	next(p, ITEM);
	if(isNext(p, OP2)){
		next(p, OP2);
		next(p, ITEM);
	}
	pop(p, "EXP");
}

void parseCond(Parser *p){							// COND = EXP COND_OP EXP
	push(p, "COND");
	parseExp(p);
	next(p, COND_OP);
	parseExp(p);
	pop(p, "COND");
}

char *level(Parser *p){								// return level of syntax tree
	return strSpaces(p->stack->count);
}

char *nextToken(Parser *p){
	return (char*)p->tokens->item[p->tokenIdx];
}

BOOL isEnd(Parser *p){								// return true if tokenIdx is at the end of tokens
	return (p->tokenIdx >= p->tokens->count);
}

BOOL isNext(Parser *p, char *pTypes){				// return true if next token is pType
	char *token = nextToken(p);
	if(token == NULL) return FALSE;
	char *type = tokenToType(token);
	char tTypes[MAX_LEN + 1];
	sprintf(tTypes, "|%s|", pTypes);
	return strPartOf(type, tTypes);
}

char *next(Parser *p, char *pTypes){				// create tree node of next token
	char *token = nextToken(p);						// get next token
	if(isNext(p, pTypes)){
		char *type = tokenToType(token);			// get token type(EBNF)
		Tree *child = TreeNew(type, token);			// create tree node of child
		Tree *parentTree = ArrayPeek(p->stack);		// get parent tree from stack
		TreeAddChild(parentTree, child);			// add child to parent tree
		printf("%s idx = %d, token = %s, type = %s\n", level(p), p->tokenIdx, token, type);
		p->tokenIdx++;
		return token;
	}else{
		printf("%s is not type %s\n", token, pTypes);
		error();
		p->tokenIdx++;
		return NULL;
	}
}

Tree *push(Parser *p, char *pType){					// create a pType tree node and push to p->stack
	printf("%s+%s\n", level(p), pType);
	Tree *tree = TreeNew(pType, "");
	ArrayPush(p->stack, tree);
	return tree;
}

Tree *pop(Parser *p, char *pType){					// pop a pType tree node from p->stack
	Tree *tree = ArrayPop(p->stack);
	printf("%s-%s\n", level(p), tree->type);
	if(strcmp(tree->type, pType) != 0){
		printf("pop(%s): should be %s\n", tree->type, pType);
		error();
	}
	if(p->stack->count > 0){
		Tree *parentTree = ArrayPeek(p->stack);
		TreeAddChild(parentTree, tree);
	}
	return tree;
}
	
