
enum InstructionsId{
	VM_ADD,
	VM_SUB,
	VM_MUL,
	VM_DIV,
	VM_MOD,
	VM_FADD,
	VM_FSUB,
	VM_FMUL,
	VM_FDIV,
	VM_SHL,
	VM_SHR,
	VM_AND,
	VM_OR,
	VM_XOR,
///////////////////
	VM_CMP,
	VM_SCMP,
	VM_NEG,
	VM_LDI,
	VM_FCMP,
	VM_FTOI,
	VM_ITOF,
	VM_NOT,
///////////////////
	VM_CALL,
	VM_JEQ,
	VM_JNE,
	VM_JGR,
	VM_JLS,
	VM_JGE,
	VM_JLE,
	VM_JMP,
	VM_SYSCALL,
///////////////////
	VM_RET,
///////////////////
	VM_LOAD,
	VM_LOADL,
	VM_LOADW,
	VM_LOADLW,
	VM_LOADB,
	VM_LOADLB,
	VM_STORE,
	VM_STOREL,
	VM_STOREW,
	VM_STORELW,
	VM_STOREB,
	VM_STORELB,
////////////////////
	VM_MEMCPY,
	
};

uint32_t Execute32Bit(struct VirtualMachine* pVM, uint32_t Instruction);


// 3 operand instructions
// 0b00011111 get the repeat modifier
// 0b00011111 get the first operand
// 0b00000001 get last integer flag
// 0b00011111 get the second operand
// get the third operand
//uint32_t result;
// repeat check
#ifndef STAT_COUNTERS
#define I3Operands_Base()\
\
	uint32_t rep = (Instruction >> 7) & 31; \
	uint32_t fop = (Instruction >> 12) & 31; \
	uint32_t last_int_flag = (Instruction >> 17) & 1; \
	uint32_t sop = (Instruction >> 18) & 31; \
	uint32_t top = (Instruction >> 23) & 511; 
#else
	#define I3Operands_Base()\
\
	uint32_t rep = (Instruction >> 7) & 31; \
	uint32_t fop = (Instruction >> 12) & 31; \
	uint32_t last_int_flag = (Instruction >> 17) & 1; \
	uint32_t sop = (Instruction >> 18) & 31; \
	uint32_t top = (Instruction >> 23) & 511; \
	\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	pVM->RegistersHit[sop] = pVM->RegistersHit[sop] + 1;\
	if(last_int_flag == 0){\
		pVM->RegistersHit[top] = pVM->RegistersHit[top] + 1;\
	}
#endif


// 2 operand instructions
// 0b00011111 get the repeat modifier
// 0b00011111 get the first operand
// 0b00000001 get last integer flag
// 0b00011111 get the second operand
#ifndef STAT_COUNTERS
#define I2Operands_Base()\
	uint32_t rep = (Instruction >> 7) & 31;\
	uint32_t fop = (Instruction >> 12) & 31;\
	uint32_t last_int_flag = (Instruction >> 17) & 1;\
	uint32_t sop = (Instruction >> 18) & 16383;
#else
#define I2Operands_Base()\
	uint32_t rep = (Instruction >> 7) & 31;\
	uint32_t fop = (Instruction >> 12) & 31;\
	uint32_t last_int_flag = (Instruction >> 17) & 1;\
	uint32_t sop = (Instruction >> 18) & 16383;\
	\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	if(last_int_flag == 0){\
		pVM->RegistersHit[sop] = pVM->RegistersHit[sop] + 1;\
	}
#endif


// 1 operand instructions
// 0b00000001 get last integer flag
#ifndef STAT_COUNTERS
#define I1Operand_Base()\
	uint32_t last_int_flag = (Instruction >> 7) & 1;\
	uint32_t fop;\
\
	if(last_int_flag == 0){\
		fop = pVM->Registers.r[(Instruction >> 8) & 31];\
		if(((uint64_t)fop + 4) > pVM->CodeSize){\
			return VM_CODE_ACCESS_VIOLATION;\
		}\
	}else{\
		fop = (Instruction >> 8) & 16777215;\
	}
#else
#define I1Operand_Base()\
	uint32_t last_int_flag = (Instruction >> 7) & 1;\
	uint32_t fop;\
\
	if(last_int_flag == 0){\
		pVM->RegistersHit[(Instruction >> 8) & 31] = pVM->RegistersHit[(Instruction >> 8) & 31] + 1;\
		fop = pVM->Registers.r[(Instruction >> 8) & 31];\
		if(((uint64_t)fop + 4) > pVM->CodeSize){\
			return VM_CODE_ACCESS_VIOLATION;\
		}\
	}else{\
		fop = (Instruction >> 8) & 16777215;\
	}
#endif
	

// 2 operand memory instructions
// 0b00011111 get the repeat modifier
// 0b00011111 get the first operand
// 0b00000001 get last integer flag
// 0b00011111 get the second operand
#ifndef STAT_COUNTERS
#define I2OperandsMem_Base()\
	uint32_t rep = ((Instruction >> 7) & 31) + 1;\
	uint32_t fop = (Instruction >> 12) & 31;\
	uint32_t last_int_flag = (Instruction >> 17) & 1;\
	uint32_t sop;\
\
	if(last_int_flag == 0){\
		sop = pVM->Registers.r[(Instruction >> 18) & 31];\
	}else{\
		sop = (Instruction >> 18) & 16383;\
	}
#else
	#define I2OperandsMem_Base()\
	uint32_t rep = ((Instruction >> 7) & 31) + 1;\
	uint32_t fop = (Instruction >> 12) & 31;\
	uint32_t last_int_flag = (Instruction >> 17) & 1;\
	uint32_t sop;\
\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	if(last_int_flag == 0){\
		sop = pVM->Registers.r[(Instruction >> 18) & 31];\
		pVM->RegistersHit[(Instruction >> 18) & 31] = pVM->RegistersHit[(Instruction >> 18) & 31] + 1;\
	}else{\
		sop = (Instruction >> 18) & 16383;\
	}
#endif

#ifndef STAT_COUNTERS
#define I2OperandsLocalMem_Base()\
	uint32_t rep = ((Instruction >> 7) & 31) + 1;\
	uint32_t fop = (Instruction >> 12) & 31;\
	uint32_t last_int_flag = (Instruction >> 17) & 1;\
	int32_t sop;\
\
	if(last_int_flag == 0){\
		sop = *(int32_t*)&pVM->Registers.r[(Instruction >> 18) & 31];\
	}else{\
		struct { signed int op:14; } s;\
		sop = (Instruction >> 18) & 16383;\
		*(int32_t*)&sop = s.op = sop;\
	}
#else
	#define I2OperandsMem_Base()\
	uint32_t rep = ((Instruction >> 7) & 31) + 1;\
	uint32_t fop = (Instruction >> 12) & 31;\
	uint32_t last_int_flag = (Instruction >> 17) & 1;\
	int32_t sop;\
\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	if(last_int_flag == 0){\
		sop = *(int32_t*)&pVM->Registers.r[(Instruction >> 18) & 31];\
		pVM->RegistersHit[(Instruction >> 18) & 31] = pVM->RegistersHit[(Instruction >> 18) & 31] + 1;\
	}else{\
		struct { signed int op:14; } s;\
		sop = (Instruction >> 18) & 16383;\
		*(int32_t*)&sop = s.op = sop;\
	}
#endif

inline uint32_t IfAvailableLocalMemory(struct VirtualMachine* pVM, uint32_t size)
{
	uint32_t LocalUsed = pVM->pCurrentLocalMemory - pVM->pLocalMemory + pVM->CurrentLocalMemorySize;
	if((LocalUsed + LOCAL_MEMORY_FRAME_START_SIZE) > pVM->LocalMemorySize){
		// need to allocate more memory
		uint32_t new_size = pVM->LocalMemorySize + 64 * LOCAL_MEMORY_FRAME_START_SIZE;
		if(new_size > MAX_ALLOWED_LOCAL_MEMORY){
			return VM_NOT_ENOUGH_MEMORY;
		}
		pVM->pLocalMemory = (uint8_t*)realloc(pVM->pLocalMemory, new_size);
		pVM->LocalMemorySize = new_size;
	}
	return VM_OK;
}

#define CheckStackSize(){\
	if(pVM->CurrentStackTop == pVM->CallStackSize){\
		pVM->CallStackSize = pVM->CallStackSize + 32;\
		if(pVM->CallStackSize >= MAX_ALLOWED_STACK_SIZE){\
			return VM_STACK_IS_TOO_BIG;\
		}\
		pVM->pCallStack = (Call*)realloc(pVM->pCallStack, sizeof(Call) * pVM->CallStackSize);\
	}\
}

#define MakeCall(){\
	pVM->MaxNegativeOffset = pVM->MaxNegativeOffset - pVM->CurrentLocalMemorySize;\
	pVM->pCurrentLocalMemory = pVM->pCurrentLocalMemory + pVM->CurrentLocalMemorySize;\
	pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize = pVM->CurrentLocalMemorySize;\
	pVM->CurrentLocalMemorySize = LOCAL_MEMORY_FRAME_START_SIZE;\
	pVM->pCallStack[pVM->CurrentStackTop].regPC = pVM->Registers.PC + 4; /* next instruction after call*/ \
	pVM->pCallStack[pVM->CurrentStackTop].regFLAGS = pVM->Registers.FLAGS;\
}