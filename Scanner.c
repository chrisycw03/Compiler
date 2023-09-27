#include "Scanner.h"
#include <string.h>

char STRING[] = "string";
char NUMBER[] = "number";
char ID[] = "id";
char KEYWORDS[] = "|if|for|while|return|";
char OP1[] = "|++|--|";
char OP2[] = "|+|-|*|/|";
char COND_OP[] = "==|!=|<=|>=|<|>|";
char ITEM[] = "|id|number|string|";
char OP[] = "+-*/<=>!";

#define ch() (scanner->text[scanner->textIdx])
#define next() (scanner->textIdx++)

Scanner *ScannerNew(char *pText){				// construct a Scanner from a program text(pText)
	Scanner *scanner = ObjNew(Scanner, 1);		// allocate a memory space of scanner
	scanner->text = pText;						// assign program text(pText) to scanner
	scanner->textLen = strlen(pText);
	scanner->textIdx = 0;
	return scanner;
}

void ScannerFree(Scanner *scanner){
	ObjFree(scanner);
}

char *ScannerScan(Scanner *scanner){
	while(strMember(ch(), SPACE))				// skip SPACE
		next();
	if(scanner->textIdx >= scanner->textLen)	// return NULL if exceed length
		return NULL;
	char c = ch();
	int begin = scanner->textIdx;
	if(c == '\"'){
		next();
		while(ch() != '\"') next();				// read until not string
		next();
	}else if(strMember(c, OP)){
		while(strMember(ch(), OP)) next();		// read until not Operator
	}else if(strMember(c, DIGIT)){
		while(strMember(ch(), DIGIT)) next();	// read until not Number
	}else if(strMember(c, ALPHA)){
		// read until not alphabat or number
		while(strMember(ch(), ALPHA) || strMember(ch(), DIGIT)) next();
	}else{
		next();
	}
	// copy read token from scanner->text to scanner->token
	strSubstr(scanner->token, scanner->text, begin, scanner->textIdx - begin);
	return scanner->token;
}
	
Array *tokenize(char *text){					// convert text to tokens
	Array *tokens = ArrayNew(10);				// create an array to store tokens
	Scanner *scanner = ScannerNew(text);
	char *token = NULL;							// current scanned token
	while((token = ScannerScan(scanner))){		// loop when token is not NULL
		ArrayAdd(tokens, newStr(token));		// add scanned token to Array tokens
		printf("token = %s\n", token);
	}
	return tokens;
}

char *tokenToType(char *token){					// get token type
	if(strPartOf(token, KEYWORDS))
		return token;
	else if(token[0] == '\"')
		return STRING;
	else if(strMember(token[0], DIGIT))
		return NUMBER;
	else if(strMember(token[0], ALPHA))
		return ID;
	else
		return token;
}

void printTokens(Array *tokens){				// print each token
	printf("tokens->count = %d\n", tokens->count);
	for(int i = 0; i < tokens->count; i++){
		char *token = tokens->item[i];
		printf("token = %s, type = %s\n", token, tokenToType(token));
	}	
}
