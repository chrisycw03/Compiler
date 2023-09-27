#ifndef CPU0_H
#define CPU0_H

#include "OpTable.h"

typedef struct {
	BYTE *m;
	int mSize;
	int R[16], IR;
} Cpu0;

void runObjFile(char *objFile);			// main function of Virtual Machine

Cpu0* Cpu0New(char *objFile);
void Cpu0Free(Cpu0 *cpu0);
void Cpu0Run(Cpu0 *cpu0, int startPC);
void Cpu0Dump(Cpu0 *cpu0);

extern void cpu0test();

#endif
