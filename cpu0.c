#include "Cpu0.h"

void runObjFile(char *objFile){						// main function of Virtual Machine
	printf("===VM0:run %s on CPU0===\n", objFile);
	Cpu0 *cpu0 = Cpu0New(objFile);					// construct CPU0
	Cpu0Run(cpu0, 0);								// execution
	Cpu0Dump(cpu0);									// dump CPU0 register
	Cpu0Free(cpu0);									// free CPU0 Object
}

Cpu0* Cpu0New(char *objFile){						// constructor of CPU0
	Cpu0 *cpu0=ObjNew(Cpu0, 1);
	cpu0->m = newFileBytes(objFile, &cpu0->mSize);	// load object file
	return cpu0;
}

void Cpu0Free(Cpu0 *cpu0){
	freeMemory(cpu0->m);
	ObjFree(cpu0);
}                                                                                         

#define bits(i, from, to) ((UINT32)i << (31 - to) >> (31 - to + from))			// get bits 'from' to 'to'
#define ROR(i, k) (((UINT32)i >> k) | (bits(i, 32 - k, 31) << (32 - k)))		// right rotate k bits
#define ROL(i, k) (((UINT32)i << k) | (bits(i, 0, k-1) << (32-k)))				// left rotate k bits
#define SHR(i, k) ((UINT32)i >> k)												// right shift k bits
#define SHL(i, k) ((UINT32)i << k)												// left shift k bits
#define bytesToInt32(p) (INT32)(p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3])		// convert 4 bytes to int
#define bytesToInt16(p) (INT16)(p[0]<<8 | p[1])									// convert 2 bytes to INT16(short)
#define int32ToBytes(i, bp) { bp[0]=i>>24; bp[1]=i>>16; bp[2]=i>>8; bp[3]=i;}	// convert int to 4 bytes
#define StoreInt32(i, m, addr) { BYTE *p = &m[addr]; int32ToBytes(i, p); }		// i = m[addr…addr+3]
#define LoadInt32(i, m, addr) { BYTE *p = &m[addr]; i = bytesToInt32(p); }		// m[addr..addr+3] = i
#define StoreByte(b, m, addr) { m[addr] = (BYTE) b; }							// m[addr] = b
#define LoadByte(b, m, addr) { b = m[addr]; }									// b = m[addr]

#define PC R[15]																// PC is R[15]
#define LR R[14]																// LR is R[14]
#define SP R[13]																// SP is R[13]
#define SW R[12]																// SW is R[12]

void cpu0test(){
	char *o0 = "test.obj0";
	Cpu0 *cpu0 = Cpu0New(o0);
	int mSize = cpu0->mSize;
	printf("%d\n", mSize);
	Cpu0Run(cpu0, 0);
	Cpu0Dump(cpu0);
	Cpu0Free(cpu0);
}

void Cpu0Run(Cpu0 *cpu0, int start){							// main function of Virtual Machine
	char buffer[200];
//	unsigned int IR, op, ra, rb, rc, cc;
	unsigned int IR, op, ra, rb, rc;
	int c5, c12, c16, c24, caddr, raddr;
	unsigned int N, Z;
	BYTE *m = cpu0->m;
	int  *R = cpu0->R;
	PC = start;													// set PC to starting address
	LR = -1;
	BOOL stop = FALSE;
	while (!stop) {
		R[0] = 0;												// R[0] = 0
		LoadInt32(IR, m, PC);									// fetch instructions and store in IR = [PC..PC+3]
		cpu0->IR = IR;
		PC += 4;												// update PC point to next instruction
		op = bits(IR, 24, 31);									// Op = IR[24..31]
		ra = bits(IR, 20, 23);									// Ra = IR[20..23]
		rb = bits(IR, 16, 19);									// Rb = IR[16..19]
		rc = bits(IR, 12, 15);									// Rc = IR[12..15]
		c5 = bits(IR,  0, 4);									// 5 bits cx
		c12= bits(IR, 0, 11);									// 12 bits cx
		c16= bits(IR, 0, 15);									// 16 bits cx
		c24= bits(IR, 0, 23);									// 24 bits cx
		N  = bits(SW, 31, 31);									// get 'N FLAG' in SW
		Z  = bits(SW, 30, 30);									// get 'Z FLAG' in SW
		if (bits(IR, 11, 11)!=0) c12 |= 0xFFFFF000;				// convert negative cx to its complement
		if (bits(IR, 15, 15)!=0) c16 |= 0xFFFF0000;				// convert negative cx to its complement
		if (bits(IR, 23, 23)!=0) c24 |= 0xFF000000;				// convert negative cx to its complement
		caddr = R[rb] + c16;									// get address: [Rb+cx]
		raddr = R[rb] + R[rc];									// get address: [Rb+Rc]
		switch (op) {
			case OP_LD : LoadInt32(R[ra], m, caddr); break;		// LD
			case OP_ST : StoreInt32(R[ra], m, caddr); break;	// ST
			case OP_LDB: LoadByte(R[ra], m, caddr); break;		// LDB
			case OP_STB: StoreByte(R[ra], m, caddr); break;		// STB
			case OP_LDR: LoadInt32(R[ra], m, raddr); break;		// LDR
			case OP_STR: StoreInt32(R[ra], m, raddr); break;	// STR
			case OP_LBR: LoadByte(R[ra], m, raddr); break;		// LBR
			case OP_SBR: StoreByte(R[ra], m, raddr); break;		// SBR
			case OP_LDI: R[ra] = c16; break;					// LDI
			case OP_CMP: {										// CMP: set N, Z flags in SW
				if (R[ra] > R[rb]) {							// > : SW(N=0, Z=0)
					SW &= 0x3FFFFFFF;							// N=0, Z=0
				} else if (R[ra] < R[rb]) {						// < : SW(N=1, Z=0, ....)                                                
					SW |= 0x80000000;							// N=1;
					SW &= 0xBFFFFFFF;							// Z=0;
				} else {										// = : SW(N=0, Z=1)                      
					SW &= 0x7FFFFFFF;							// N=0;
					SW |= 0x40000000;							// Z=1;
				}
				ra = 12;
				break;                                                                                        
			}
			case OP_MOV: R[ra] = R[rb]; break;						// MOV
			case OP_ADD: R[ra] = R[rb] + R[rc]; break;				// ADD
			case OP_SUB: R[ra] = R[rb] - R[rc]; break;				// SUB
			case OP_MUL: R[ra] = R[rb] * R[rc]; break;				// MUL
			case OP_DIV: R[ra] = R[rb] / R[rc]; break;				// DIV
			case OP_AND: R[ra] = R[rb] & R[rc]; break;				// AND
			case OP_OR:  R[ra] = R[rb] | R[rc]; break;				// OR
			case OP_XOR: R[ra] = R[rb] ^ R[rc]; break;				// XOR
			case OP_ROL: R[ra] = ROL(R[rb],c5); break;				// ROL
			case OP_ROR: R[ra] = ROR(R[rb],c5); break;				// ROR
			case OP_SHL: R[ra] = SHL(R[rb],c5); break;				// SHL
			case OP_SHR: R[ra] = SHR(R[rb],c5); break;				// SHR
			case OP_JEQ: if(Z == 1) PC += c24; break;				// JEQ Z=1
			case OP_JNE: if(Z == 0) PC += c24; break;				// JNE Z=0
			case OP_JLT: if(N == 1 && Z == 0) PC += c24; break;		// JLT NZ=10
			case OP_JGT: if(N == 0 && Z == 0) PC += c24; break;		// JGT NZ=00
			case OP_JLE:											// JLE NZ=10 or 01
				if((N == 1 && Z == 0) || (N == 0 && Z == 1)) PC+=c24; break;
			case OP_JGE:											// JGE NZ=00 or 01
				if((N == 0 && Z == 0) || (N == 0 && Z == 1)) PC+=c24; break;
			case OP_JMP: PC += c24; break;							// JMP
			case OP_SWI: LR = PC; PC = c24; break;					// SWI
			case OP_JSUB:LR = PC; PC += c24; break;					// JSUB
			case OP_RET: if(LR < 0) stop = TRUE; else PC=LR; break;	// RET
			case OP_PUSH:SP -= 4; StoreInt32(R[ra], m, SP); break;	// PUSH
			case OP_POP: LoadInt32(R[ra], m, SP); SP += 4; break;	// POP
			case OP_PUSHB:SP--; StoreByte(R[ra], m, SP); break;		// PUSH
			case OP_POPB:LoadByte(R[ra], m, SP); SP++; break;		// POPB
			default: printf("Error:invalid op (%02x) ", op);
		}

		sprintf(buffer, "PC=%08x IR=%08x SW=%08x R[%02d]=0x%08x=%d\n", PC, IR, SW, ra, R[ra], R[ra]);
		strToUpper(buffer);
		printf(buffer);
	}                                                                                                   
}                                                                                       

void Cpu0Dump(Cpu0 *cpu0) {										// dump registers of CPU0
	printf("\n===CPU0 dump registers===\n");
	printf("IR =0x%08x=%d\n", cpu0->IR, cpu0->IR);				// print IR
	for (int i = 0; i < 16; i++)								// print R0 to R15
		printf("R[%02d]=0x%08x=%d\n",i,cpu0->R[i],cpu0->R[i]);
}
