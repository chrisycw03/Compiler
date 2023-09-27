#ifndef TREE_H
#define TREE_H

#include "Array.h"

typedef struct{			// Tree object
	char *type;			// type of tree
	char *value;		// value of tree
	Array *childs;		// children of tree
}Tree;

Tree *TreeNew(char *pType, char *pValue);
void TreeFree(Tree *tree);
void TreeAddChild(Tree *tree, Tree *child);
void TreePrint(Tree *tree);

#endif
