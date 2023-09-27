#include "Assembler.h"

void assemble(char *asmFile, char *objFile) {									// main function of assembler
	printf("Assembler:asmFile = %s objFile = %s\n", asmFile, objFile);			// imported assembly file name; exported object file name
	printf("====================Assemble====================\n");
	char *text = newFileStr(asmFile);											// read text in asmFile
	Assembler *a = AsmNew();
	AsmPass1(a, text); 															// 1st pass of assembler
	printf("==================SYMBOL TABLE==================\n");
	HashTableEach(a->symTable, (FuncPtr1) AsmCodePrintln);						// print Symbol Table
	AsmPass2(a);																// 2nd pass of assembler
	AsmSaveObjFile(a, objFile);													// export assembly file
	AsmFree(a);																	// free assembler
	freeMemory(text);
}

Assembler* AsmNew() {
	Assembler *a = ObjNew(Assembler, 1);
	a->codes = ArrayNew(10);
	a->symTable = HashTableNew(127);
	a->opTable = OpTableNew();
	return a;
}

void AsmFree(Assembler *a) {
	ArrayFree(a->codes, (FuncPtr1) AsmCodeFree);
	HashTableFree(a->symTable);
	OpTableFree();
	ObjFree(a);
}

void AsmPass1(Assembler *a, char *text) {										// 1st pass of assembler
	int address = 0;															// start address of memory
	Array* lines = split(text, "\r\n", REMOVE_SPLITER);							// split each line in asmFile
	ArrayEach(lines, strPrintln);
	printf("=====================PASS1====================\n");
	for (int i = 0; i < lines->count; i++){
		strReplace(lines->item[i], SPACE, ' ');									// replace " \t\n\r" to ' '
		AsmCode *code = AsmCodeNew(lines->item[i]);								// constructor of AsmCode(initialize label, op, args by line->item[i])
		code->address = address;												// set address of current line

		Op *op = HashTableGet(opTable, code->op);								// get corresponded OP CODE
		if(op != NULL){
			code->opCode = op->code;											// set OP CODE
			code->type = op->type;												// set type of OP CODE
		}

		if(strlen(code->label) > 0)												// if label exist, put to symTable(HashTable): (label name, AsmCode)
			HashTablePut(a->symTable, code->label, code);

		ArrayAdd(a->codes, code);												// add AsmCode to a->code
		AsmCodePrintln(code);													// print AsmCode
		code->size = AsmCodeSize(code);											// calculate memory size of AsmCode
		address += code->size;													// update next code address
	}
	ArrayFree(lines, strFree);													// free memory
}


void AsmPass2(Assembler *a){													// 2nd pass of assembler
	printf("======================PASS2=====================\n");
	for (int i = 0; i < a->codes->count; i++) {									// encode each line of AsmCode
		AsmCode *code = a->codes->item[i];
		AsmTranslateCode(a, code);
		AsmCodePrintln(code);
	}
}

void AsmTranslateCode(Assembler *a, AsmCode *code) {							// function to encode AsmCode
	char p1[100], p2[100], p3[100], pt[100];
	int ra = 0, rb = 0, rc = 0, cx = 0;
	char cxCode[9] = "00000000", objCode[100] = "", args[100] = "";
	strcpy(args, code->args);
	strReplace(args, ",", ' ');
	int pc = code->address + 4;													// update PC after fetch the instruction
	switch (code->type) {														// process instructions by its type
		case 'J' :																// **J type instruction: Op + Cx
			if (!strEqual(args, "")) {                                                                    
				AsmCode *labelCode = HashTableGet(a->symTable, args);			// get argument code from symTable
				cx = labelCode->address - pc;									// calculate 'Cx'
				sprintf(cxCode, "%8x", cx);
			}                                                                                             
			sprintf(objCode, "%2x%s", code->opCode, &cxCode[2]);				// output Object Code
			break;
		case 'L' :																// **L type instruction: OP + Ra + [Rb + Cx]
			sscanf(args, "R%d %s", &ra, p2);									// p2:[Rb + Cx]
			if(strHead(p2, "[")){
				sscanf(p2, "[R%d+%s]", &rb, pt);								// pt: Cx
				if(sscanf(pt, "R%d", &rc) <= 0)									// pt: Rc | Cx
					sscanf(pt, "%d", &cx);
			}else if(sscanf(p2, "%d", &cx) > 0){								// Rb is R0 (p2:[R0 + Cx])
			}else{																// Cx is label
				AsmCode *labelCode = HashTableGet(a->symTable, p2);
				cx = labelCode->address - pc;									// calculate gap between PC and label
				rb = 15; 														// R[15] is PC
			}
			sprintf(cxCode, "%8x", cx);
			sprintf(objCode, "%2x%x%x%s", code->opCode, ra, rb, &cxCode[4]);	// output Object Code
			break;
		case 'A' :																// **A type intruction: OP + Ra + Rb + Rc + Cx
			sscanf(args, "%s %s %s", p1, p2, p3);								// p1: Ra, p2: Rb, p3: Rc + Cx
			sscanf(p1, "R%d", &ra);
			sscanf(p2, "R%d", &rb);
			if (sscanf(p3, "R%d", &rc) <= 0)									// p3: Rc | Cx
				sscanf(p3, "%d", &cx);
			sprintf(cxCode, "%8x", cx);                                                                   
			sprintf(objCode, "%2x%x%x%x%s", code->opCode,ra,rb,rc,&cxCode[5]);	// output Object Code
			break;
		case 'D' : {															// **D type instruction(Data): RESW, RESB, WORD, BYTE
			char format4[]="%8x", format1[]="%2x", *format = format1;
			switch (code->opCode) {												// RESW | RESB
				case OP_RESW:
				case OP_RESB:
					memset(objCode, '0', code->size * 2);						// Byte convert to digits(HEX): 1 Byte = 2 digits
					objCode[code->size * 2] = '\0';
					break;
				case OP_WORD:
					format = format4;											// set format to %8x (4 BYTE)
				case OP_BYTE: {													// set format to %2x (1 BYTE)
					Array *array = split(args, " ", REMOVE_SPLITER);
					char *objPtr = objCode;
					for(int i = 0; i < array->count; i++) {						// convert each parameters to objCode
						char *item = array->item[i];
						if(isdigit(item[0]))									// if parameter is digit
							sprintf(objPtr, format, atoi(item));
						else{													// parameter points to a label
							AsmCode *itemCode = HashTableGet(a->symTable, item);
							sprintf(objPtr, format, itemCode->address);			// objCode points to label's address
						}
						objPtr += strlen(objPtr);								// next parameter
					}
					ArrayFree(array, strFree);
					break;
				}
			}
			break;
		}
		default:
			strcpy(objCode, "");
			break;
	}
	strReplace(objCode, " ", '0');
	strToUpper(objCode);
	code->objCode = newStr(objCode);
}

void AsmSaveObjFile(Assembler *a, char *objFile) {
	printf("============Save to ObjFile:%s===========\n", objFile);
	FILE *file = fopen(objFile, "wb");
	int i;
	for(i = 0; i < a->codes->count; i++) {
		AsmCode *code = a->codes->item[i];
		char *objPtr = code->objCode;
		while (*objPtr != '\0') {
			int x;
			sscanf(objPtr, "%2x", &x);
			assert(x >= 0 && x < 256);
			BYTE b = (BYTE) x;
			fwrite(&b, sizeof(BYTE), 1, file);
			objPtr += 2;
			char bstr[3];
			sprintf(bstr, "%2x", b);
			strReplace(bstr, " ", '0');
			strToUpper(bstr);
			printf("%s", bstr);
		}
	}
	printf("\n");
	fclose(file);
}

void AsmCodePrintln(AsmCode *code) {
	char label[100] = "";
	char address[100];
	char buffer[200];
	if(strlen(code->label) > 0)
		sprintf(label, "%s:", code->label);
	sprintf(address, "%4x", code->address);
	strReplace(address, " ", '0');
	sprintf(buffer, "%4s %-8s %-4s %-14s %c %2x %s\n", address, label, code->op, code->args, code->type, code->opCode, code->objCode);
	strToUpper(buffer);
	printf(buffer);
}

AsmCode *AsmCodeNew(char *line){												// construct a AsmCode
	AsmCode* code = ObjNew(AsmCode, 1);
	char label[100]="";
	char op[100]="";
	char args[100]="";
	char temp[100];

	int count = sscanf(line, "%s %s %[^;]", label, op, args);
	assert(count != EOF);														// check 'sscanf' is success

	if(strTail(label, ":")){													// if ':' exist, label exist
		strTrim(temp, label, ":");												// remove ':' and copy to temp
		strcpy(label, temp);
	}else{																		// label did not exist
		strcpy(label, "");														// delete label
		sscanf(line, "%s %[^;]", op, args);
	}

	code->label = newStr(label);
	code->op = newStr(op);
	strTrim(temp, args, SPACE);													// remove 'SPACE' at front and end of args
	code->args = newStr(temp);
	code->type = ' ';
	code->opCode = OP_NULL;
//	  AsmCodePrintln(code);
	return code;
}

void AsmCodeFree(AsmCode *code) {
	freeMemory(code->label);
	freeMemory(code->op);
	freeMemory(code->args);
	freeMemory(code->objCode);
	freeMemory(code);
}

int AsmCodeSize(AsmCode *code){													// calculate memory size of AsmCode
	switch (code->opCode){
		case OP_RESW :															// **RESW
			return 4 * atoi(code->args);										// size = 4 * args (BYTES)
		case OP_RESB :															// **RESB
			return atoi(code->args);											// size = args (BYTES)
		case OP_WORD :															// **WORD
			return 4 * (strCountChar(code->args, ",") + 1);						// size = 4 * (number of parameters) (BYTES)
		case OP_BYTE :															// **BYTE
			return strCountChar(code->args, ",") + 1;							// size = number of parameters (BYTES)
		case OP_NULL :															// **label
			return 0;															// size of label = 0 (BYTES)
		default :																// **Other Instructions
			return 4;															// size = 4 BYTES
	}                                                   
}
