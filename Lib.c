#include "Lib.h"

char SPLITER[] = " \t\n\r~!@#$%^&*()_+{}:\"<>?-=[]|\\;',./";
char SPACE[] = " \t\n\r";
char ALPHA[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
char DIGIT[] = "0123456789";
char NAME_CHAR[] = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

int newMemoryCount = 0;					// counter of memory allocation
void *newMemory(int size){				// function of memory allocation
	void *ptr = malloc(size);
	assert(ptr != NULL);
	memset(ptr, 0, size);
	// printf("memGet:%p\n", ptr);
	newMemoryCount++;
	return ptr;
}

int freeMemoryCount = 0; 				// function of memory free
void freeMemory(void *ptr){
	// printf("memFree:%p\n", ptr);
	free(ptr);
	freeMemoryCount++;
}

void checkMemory(){
	printf("newMemoryCount=%d freeMemoryCount=%d\n", newMemoryCount, freeMemoryCount);
}

// file input and output
BYTE *newFileBytes(char *fileName, int *sizePtr){		// return file content and length
	FILE *file = fopen(fileName, "rb");
	fseek(file, 0, SEEK_END); 							// move to the end of file
	long size = ftell(file);							// get file length
	rewind(file); 										// move to the begin of file
	BYTE *buffer = (unsigned char*)newMemory(size + 1);
	fread(buffer, size, 1, file);						// copy file content to buffer
	fclose(file);
	*sizePtr = size;
	return buffer;
}

char *newFileStr(char *fileName){ 						// return file content in string
	int size;
	BYTE *buffer = newFileBytes(fileName, &size);
	buffer[size] = '\0';
	return (char*)buffer;
}

char *newStr(char *str){ 								// create memory space to store str
	char *rzStr = newMemory(strlen(str) + 1);
	strcpy(rzStr, str);
	return rzStr;
}

char *newSubstr(char *str, int i, int len){				// create memory space to store sub string of str
	char *rzStr = newMemory(len + 1);
	strSubstr(rzStr, str, i, len);						// copy from str[i] with length 'len'
	return rzStr;
}

// functions of string
void strPrint(void *data){
	printf("%s ", (char*)data);
}

void strPrintln(void *data){
	printf("%s\n", (char*)data);
}

BOOL strHead(char *str, char *head){
	return (strncmp(str, head, strlen(head)) == 0);
}

BOOL strTail(char *str, char *tail){
	int strLen = strlen(str);
	int tailLen = strlen(tail);
	if(strLen < tailLen) return FALSE;
	return (strcmp(str + strLen - tailLen, tail) == 0);
}

int strCountChar(char *str, char *charSet){ 			// number of char from 'charSet' in 'str'
	int count = 0;
	for(int i = 0; i < strlen(str); i++)
		if(strMember(str[i], charSet))
			count++;
	return count;
}

void strSubstr(char *substr, char *str, int i, int len){ // copy string to 'substr' from str[i] with length 'len'
	strncpy(substr, &str[i], len);
	substr[len] = '\0';
}

BOOL strPartOf(char *token, char *set){					 // check if 'token' is part of 'set'
	ASSERT(token != NULL && set != NULL);
	ASSERT(strlen(token) < 100);
	char ttoken[100];
	sprintf(ttoken, "|%s|", token);
	return(strstr(set, ttoken) != NULL);
}

void strTrim(char *trimStr, char *str, char *set){		// remove char from 'set' at the begin and end of 'str'
	char *start;
	char *stop;
	for(start = str; strMember(*start, set); start++);
	for(stop = str + strlen(str) - 1; stop > str && strMember(*stop, set); stop--);
	if(start <= stop){
		strncpy(trimStr, start, stop - start + 1);		// copy string which is not char in 'set' to 'trimStr'
		trimStr[stop - start + 1] = '\0';
	}else
		strcpy(trimStr, "");
}

void strReplace(char *str, char *from, char to){		// replace "from" to "to" in str
	for(int i = 0; i < strlen(str); i++)
		if(strMember(str[i], from))
			str[i] = to;
}

char tspaces[MAX_LEN];
char *strSpaces(int level){								// create string contains spaces
	assert(level < MAX_LEN);
	memset(tspaces, ' ', MAX_LEN);
	tspaces[level] = '\0';
	return tspaces;
}

void strToUpper(char *str){								// convert string to upper case
	for(int i = 0; i < strlen(str); i++)
		str[i] = toupper(str[i]);
	
}
