#ifndef ARRAY_H
#define ARRAY_H

#include "Lib.h"

typedef struct{
	int size;		// upper limit of current array
	int count;		// elements of current array
	void **item;	// pointers of element of the array
}Array;

typedef enum{
	KEEP_SPLITER, REMOVE_SPLITER
}SplitMode;

extern void ArrayTest();

extern Array *ArrayNew(int size); 								// create new array
extern void ArrayFree(Array *array, FuncPtr1 freeFuncPtr);		// free array
extern void ArrayAdd(Array *array, void *item); 				// add 1 element to array
extern void ArrayPush(Array *array, void *item);				// push 1 element to array
extern void *ArrayPop(Array *array); 							// pop 1 element element to array
extern void *ArrayPeek(Array *array); 							// get top element
extern void *ArrayLast(Array *array); 							// get last element
extern void ArrayEach(Array *array, FuncPtr1 f); 				// call function f for each elements
extern int ArrayFind(Array *array, void *data, FuncPtr2 fcmp);	// find data in array

extern Array *split(char *str, char *spliter, SplitMode mode);

#endif
