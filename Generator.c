#include "Generator.h"

//char nullVar[100] = "";

void generate(Tree *tree, char *asmFile){			// export an assembly language file
	char nullVar[100] = "";
	Generator *g = GenNew();						// construct Generator
	g->asmFile = fopen(asmFile, "w");
	puts("===============PCODE===============");
	GenCode(g, tree, nullVar);						// Generate assembly code
	GenData(g);										// Generate Symbol Table
	fclose(g->asmFile);
	GenFree(g);
	char *asmText = newFileStr(asmFile);			// read and print assembly file
	puts("===============AsmFile===============");
	printf("read %s...\n\n", asmFile);
	printf("%s\n", asmText);
	freeMemory(asmText);
}

Generator *GenNew(){								// constructor of Generator
	Generator *g = ObjNew(Generator, 1);
	g->symTable = HashTableNew(127);
	return g;
}

void GenFree(Generator *g){
	HashTableFree(g->symTable);
	ObjFree(g);
}

char nullVar[100] = "";
Tree *GenCode(Generator *g, Tree *node, char *rzVar){	// function to generate assembly code
														// rzVar: return variable
	if(node == NULL) return NULL;						// condition of the end of recursion

	if(strEqual(node->type, "FOR")){					// FOR := 'for' '(' STMT ';' COND ';' STMT ')' BLOCK
		char forBeginLabel[100];
		char forEndLabel[100];
		char condOp[100];
		Tree *stmt1 = node->childs->item[2];
		Tree *cond = node->childs->item[4];
		Tree *stmt2 = node->childs->item[6];
		Tree *block = node->childs->item[8];
		GenCode(g, stmt1, nullVar);						// generate STMT
		int tempForCount = g->forCount++;				// count level of FOR LOOP
		sprintf(forBeginLabel, "FOR%d", tempForCount);	// create for label: entry
		sprintf(forEndLabel, "_FOR%d", tempForCount);	// create for label: exit
		GenPcode(g, forBeginLabel, "", "", "", "");		// generate Pcode
		GenCode(g, cond, condOp);						// generate COND, get condition operator
		char negOp[100];
		negateOp(condOp, negOp);
		GenPcode(g, "", "J", negOp, "", forEndLabel);	// escape FOR LOOP
		GenCode(g, block, nullVar);
		GenCode(g, stmt2, nullVar);
		GenPcode(g, "", "J", "", "", forBeginLabel);	// jump to entry of FOR LOOP for next cycle
		GenPcode(g, forEndLabel, "", "", "", "");
		return NULL;
	}else if(strEqual(node->type, "STMT")){				// STMT := return id | id '=' EXP | id ('++' | '--')
		Tree *c1 = node->childs->item[0];				// get first token of STMT
		if(strEqual(c1->type, "return")){				// STMT := return id
			Tree *id = node->childs->item[1];
			GenPcode(g, "", "RET", "", "", id->value);
		}else{
			Tree *id = node->childs->item[0];
			Tree *op = node->childs->item[1];
			if(strEqual(op->type, "=")){				// STMT := id '=' EXP
				Tree *exp = node->childs->item[2];
				char expVar[100];
				GenCode(g, exp, expVar);				// generate EXP, get expression variable
				GenPcode(g, "", "=", expVar, "", id->value);
				HashTablePut(g->symTable, id->value, id->value);	// input 'id' to symTable
				strcpy(rzVar, expVar);
			}else{
				char addsub[100];
				if(strEqual(op->value, "++"))
					strcpy(addsub, "+");
				else
					strcpy(addsub, "-");
				GenPcode(g, "", addsub, id->value, "1", id->value);
				strcpy(rzVar, id->value);
			}
		}
	}else if(strEqual(node->type, "COND")){				// COND := EXP ('=='|'!='|'<='|'>='|'>'|'<') EXP
		Tree *op = node->childs->item[1];
		char expVar1[100];
		char expVar2[100];
		GenCode(g, node->childs->item[0], expVar1);
		GenCode(g, node->childs->item[2], expVar2);
		GenPcode(g, "", "CMP", expVar1, expVar2, nullVar);
		strcpy(rzVar, op->value);						// return COND OPERATOR
	}else if(strPartOf(node->type, "|EXP|")){			// EXP := ITEM ([+-*/] ITEM)?
		Tree *item1 = node->childs->item[0];
		char var1[100];
		char var2[100];
		char tempVar[100];
		GenCode(g, item1, var1);
		if(node->childs->count > 1){
			Tree *op = node->childs->item[1];
			Tree *item2 = node->childs->item[2];
			GenCode(g, item2, var2);
			GenTempVar(g, tempVar);
			GenPcode(g, "", op->value, var1, var2, tempVar);
			strcpy(var1, tempVar);
		}
		strcpy(rzVar, var1);
	}else if(strPartOf(node->type, "|number|id|")){		// ITEM := number | id
		strcpy(rzVar, node->value);
	}else if(node->childs != NULL){						// other types of token
		for(int i = 0; i < node->childs->count; i++)
			GenCode(g, node->childs->item[i], nullVar);
	}
	return NULL;
}

void GenData(Generator *g){
	Array *symArray = HashTableToArray(g->symTable);
	for(int i = 0; i < symArray->count; i++){
		char *varName = symArray->item[i];
		char varLabel[100];
		sprintf(varLabel, "%s:", varName);
		GenAsmCode(g, varLabel, "RESW", "1", "", "");
	}
	for(int i = 0; i < g->varCount; i++){
		char tVarLabel[100];
		sprintf(tVarLabel, "T%d:", i);
		GenAsmCode(g, tVarLabel, "ReSW", "1", "", "");
	}
	ArrayFree(symArray, NULL);
}

void GenPcode(Generator *g, char *label, char *op, char *p1, char *p2, char *pTo){
	char labelTemp[100];
	if(strlen(label) > 0)
		sprintf(labelTemp, "%s:", label);
	else
		strcpy(labelTemp, "");
	printf("%-8s %-4s %-4s %-4s %-4s\n", labelTemp, op, p1, p2, pTo);
	GenPcodeToAsm(g, labelTemp, op, p1, p2, pTo);
}

void GenPcodeToAsm(Generator *g, char *label, char *op, char *p1, char *p2, char *pTo){
	if(strlen(label) > 0)
		GenAsmCode(g, label, "", "", "", "");
	if(strEqual(op, "=")){
		GenAsmCode(g, "", "LD", "R1", p1, "");
		GenAsmCode(g, "", "ST", "R1", pTo, "");
	}else if(strPartOf(op, "|+|-|*|/|")){
		char asmOp[100];
		if(strEqual(op, "+")) strcpy(asmOp, "ADD");
		else if(strEqual(op, "-")) strcpy(asmOp, "SUB");
		else if(strEqual(op, "*")) strcpy(asmOp, "MUL");
		else if(strEqual(op, "/")) strcpy(asmOp, "DIV");
		GenAsmCode(g, "", "LD", "R1", p1, "");
		GenAsmCode(g, "", "LD", "R2", p2, "");
		GenAsmCode(g, "", asmOp, "R3", "R1", "R2");
		GenAsmCode(g, "", "ST", "R3", pTo, "");
	}else if(strEqual(op, "CMP")){
		GenAsmCode(g, "", "LD", "R1", p1, "");
		GenAsmCode(g, "", "LD", "R2", p2, "");
		GenAsmCode(g, "", "CMP", "R1", "R2", "");
	}else if(strEqual(op, "J")){
		char asmOp[100];
		if(strEqual(p1, "==")) strcpy(asmOp, "JEQ");
		else if(strEqual(p1, "!=")) strcpy(asmOp, "JNE");
		else if(strEqual(p1, "<")) strcpy(asmOp, "JLT");
		else if(strEqual(p1, ">")) strcpy(asmOp, "JGT");
		else if(strEqual(p1, "<=")) strcpy(asmOp, "JLE");
		else if(strEqual(p1, ">=")) strcpy(asmOp, "JGE");
		else strcpy(asmOp, "JMP");
		GenAsmCode(g, "", asmOp, pTo, "", "");
	}else if(strEqual(op, "RET")){
		GenAsmCode(g, "", "LD", "R1", pTo, "");
		GenAsmCode(g, "", "RET", "", "", "");
	}
}

void GenAsmCode(Generator *g, char *label, char *op, char *p1, char *p2, char *pTo){
	char *realOp = op;
	if(strEqual(op, "LD"))
		if(isdigit(p2[0]))
			realOp = "LDI";
	fprintf(g->asmFile, "%-8s %-4s %-4s %-4s %-4s\n", label, realOp, p1, p2, pTo);
}

void GenTempVar(Generator *g, char *tempVar){
	sprintf(tempVar, "T%d", g->varCount++);
}

void negateOp(char *condOp, char *negOp){
	if(strEqual(condOp, "==")) strcpy(negOp, "!=");
	if(strEqual(condOp, "!=")) strcpy(negOp, "==");
	if(strEqual(condOp, ">=")) strcpy(negOp, "<");
	if(strEqual(condOp, "<=")) strcpy(negOp, ">");
	if(strEqual(condOp, ">")) strcpy(negOp, "<=");
	if(strEqual(condOp, "<")) strcpy(negOp, ">=");
}
