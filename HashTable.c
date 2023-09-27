#include "HashTable.h"

void HashTableTest(){
	printf("\n========HashTableTest()========\n");
	char *names[] = {"John", "Mary", "George", "Mickey", "Snoopy", "Bob", "Jack"};
	char *ids[] = {"1", "2", "3", "4", "5", "6", "7"};
	HashTable *table = HashTableNew(3);
	for(int i = 0; i < 5; i++){
		HashTablePut(table, names[i], ids[i]);
		printf("%s\n", table->item[i]);
	}
	for(int i = 0; i < 7; i++)
		printf("id=%s\n", HashTableGet(table, names[i]));
	HashTableEach(table, strPrintln);
	HashTableFree(table);
}

int hash(char *key, int range){				// convert key to hash code
	int hashCode = 0;
	for(int i = 0; i < strlen(key); i++){
		BYTE value = (BYTE)key[i];
		hashCode += value;
		hashCode %= range;
	}
	return hashCode;
}

Entry *EntryNew(char *key, void *data){		// create Entry{key, data}
	Entry *e = ObjNew(Entry, 1);
	e->key = key;
	e->data = data;
	return e;
}

void EntryFree(Entry *e){					// release Entry
	ObjFree(e);
}

int EntryCompare(Entry *e1, Entry *e2){
	return strcmp(e1->key, e2->key);
}

HashTable *HashTableNew(int size){			// construct a hash table
	Array *table = ArrayNew(size);
	for(int i = 0; i < table->size; i++)
		ArrayAdd(table, ArrayNew(1));
	return table;
}

void HashTableFree(HashTable *table){
	for(int ti = 0; ti < table->size; ti++){
		Array *hitArray = table->item[ti];
		ArrayFree(hitArray, (FuncPtr1) EntryFree);
	}
	ArrayFree(table, NULL);
}

Entry keyEntry;
void *HashTableGet(HashTable *table, char *key){				// find element in hash table by key and return
	int slot = hash(key, table->size);
	Array *hitArray = (Array*)table->item[slot];
	keyEntry.key = key;
	int keyIdx = ArrayFind(hitArray, &keyEntry, EntryCompare);
	if(keyIdx >= 0){
		Entry *e = hitArray->item[keyIdx];
		return e->data;
	}
	return NULL;
}

void HashTablePut(HashTable *table, char *key, void *data){		// add (key, data) into hash table
	Entry *e;
	int slot = hash(key, table->size); 							// get HashCode
	Array *hitArray = (Array*)table->item[slot];				
	keyEntry.key = key;
	int keyIdx = ArrayFind(hitArray, &keyEntry, EntryCompare);
	if(keyIdx >= 0){
		e = hitArray->item[keyIdx];
		e->data = data;
	}else{
		e = EntryNew(key, data);
		ArrayAdd(hitArray, e);
	}
}

void HashTableEach(HashTable *table, FuncPtr1 f){
	for(int i = 0; i < table->count; i++){
		Array *hits = table->item[i];
		for(int j = 0; j < hits->count; j++){
			Entry *e = hits->item[j];
			f(e->data);
		}
	}
}

Array *HashTableToArray(HashTable *table){			// convert hash table to array
	Array *array = ArrayNew(table->count);
	for(int i = 0; i < table->count; i++){
		Array *hits = table->item[i];
		for(int j = 0; j < hits->count; j++){
			Entry *e = hits->item[j];
			ArrayAdd(array, e->data);
		}
	}
	return array;
}
