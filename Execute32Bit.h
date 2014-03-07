#define INT_FLAG 1
enum InstructionsId{
	VM_ADD,
	VM_ADDI,
	VM_SUB,
	VM_SUBI,
	VM_MUL,
	VM_MULI,
	VM_DIV,
	VM_DIVI,
	VM_MOD,
	VM_MODI,
	VM_FADD,
	VM_FADDI,
	VM_FSUB,
	VM_FSUBI,
	VM_FMUL,
	VM_FMULI,
	VM_FDIV,
	VM_FDIVI,
	VM_SHL,
	VM_SHLI,
	VM_SHR,
	VM_SHRI,
	VM_AND,
	VM_ANDI,
	VM_OR,
	VM_ORI,
	VM_XOR,
	VM_XORI,
///////////////////
	VM_CMP,
	VM_CMPI,
	VM_SCMP,
	VM_SCMPI,
	VM_NEG,
	VM_NEGI,
	VM_FCMP,
	VM_FCMPI,
	VM_FTOI,
	VM_FTOII,
	VM_ITOF,
	VM_ITOFI,
	VM_NOT,
	VM_NOTI,
///////////////////
	VM_CALL,
	VM_CALLI,
	VM_JEQ,
	VM_JEQI,
	VM_JNE,
	VM_JNEI,
	VM_JGR,
	VM_JGRI,
	VM_JLS,
	VM_JLSI,
	VM_JGE,
	VM_JGEI,
	VM_JLE,
	VM_JLEI,
	VM_JMP,
	VM_JMPI,
///////////////////
	VM_LOAD,
	VM_LOADI,
	VM_LOADW,
	VM_LOADWI,
	VM_LOADB,
	VM_LOADBI,
	VM_STORE,
	VM_STOREI,
	VM_STOREW,
	VM_STOREWI,
	VM_STOREB,
	VM_STOREBI,
// local
	VM_LOADL,
	VM_LOADLI,
	VM_LOADLW,
	VM_LOADLWI,
	VM_LOADLB,
	VM_LOADLBI,
	VM_STOREL,
	VM_STORELI,
	VM_STORELW,
	VM_STORELWI,
	VM_STORELB,
	VM_STORELBI,
///////////////////
	VM_LDI,
	VM_MOV,
	VM_COPY,
	VM_SYSCALL,
	VM_RET,
////////////////////
	VM_MEMCPY,
	
};

//uint32_t Execute32Bit(struct VirtualMachine* pVM, uint32_t Instruction);


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
	uint32_t rep = (Instruction >> 8) & 31; \
	uint32_t fop = (Instruction >> 13) & 31; \
	uint32_t sop = (Instruction >> 18) & 31; \
	uint32_t top = (Instruction >> 23)/* & 511*/; 
#else
	#define I3Operands_Base()\
\
	uint32_t rep = (Instruction >> 8) & 31; \
	uint32_t fop = (Instruction >> 13) & 31; \
	uint32_t sop = (Instruction >> 18) & 31; \
	uint32_t top = (Instruction >> 23); \
	\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	pVM->RegistersHit[sop] = pVM->RegistersHit[sop] + 1;\
	if((id & 1) == 0){\
		pVM->RegistersHit[top] = pVM->RegistersHit[top] + 1;\
	}\

#endif

#ifndef STAT_COUNTERS
#define I3OperandsInt_Base()\
\
	uint32_t fop = (Instruction >> 8) & 31; \
	uint32_t sop = (Instruction >> 13) & 31; \
	uint32_t top = (Instruction >> 18)/* & 511*/; 
#else
	#define I3OperandsInt_Base()\
\
	uint32_t fop = (Instruction >> 8) & 31; \
	uint32_t sop = (Instruction >> 13) & 31; \
	uint32_t top = (Instruction >> 18); \
	\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	pVM->RegistersHit[sop] = pVM->RegistersHit[sop] + 1;\
	if((id & 1) == 0){\
		pVM->RegistersHit[top] = pVM->RegistersHit[top] + 1;\
	}\

#endif

// 2 operand instructions
// 0b00011111 get the repeat modifier
// 0b00011111 get the first operand
// 0b00000001 get last integer flag
// 0b00011111 get the second operand
#ifndef STAT_COUNTERS
#define I2Operands_Base()\
	uint32_t rep = (Instruction >> 8) & 31;\
	uint32_t fop = (Instruction >> 13) & 31;\
	uint32_t sop = (Instruction >> 18)/* & 16383*/;
#else
#define I2Operands_Base()\
	uint32_t rep = (Instruction >> 8) & 31;\
	uint32_t fop = (Instruction >> 13) & 31;\
	uint32_t sop = (Instruction >> 18);\
	\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	pVM->RegistersHit[sop] = pVM->RegistersHit[sop] + 1;\
	
#endif

#ifndef STAT_COUNTERS
#define I2OperandsInt_Base()\
	uint32_t fop = (Instruction >> 8) & 31;\
	uint32_t sop = (Instruction >> 13)/* & 16383*/;
#else
#define I2OperandsInt_Base()\
	uint32_t fop = (Instruction >> 8) & 31;\
	uint32_t sop = (Instruction >> 13)/* & 16383*/;\
	\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	
#endif

// 1 operand instructions
// 0b00000001 get last integer flag
#ifndef STAT_COUNTERS
#define I1Operand_Base()\
	uint32_t fop = Instruction >> 8;\
\
	fop = pVM->Registers.r[fop];\
	if(((uint64_t)fop + 4) > pVM->CodeSize){\
		assert(false);\
		return VM_CODE_ACCESS_VIOLATION;\
	}
#else
#define I1Operand_Base()\
	uint32_t fop = (Instruction >> 8);\
\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	fop = pVM->Registers.r[fop];\
	if(((uint64_t)fop + 4) > pVM->CodeSize){\
		assert(false);\
		return VM_CODE_ACCESS_VIOLATION;\
	}
#endif

#ifndef STAT_COUNTERS
#define I1OperandInt_Base()\
	uint32_t fop = (Instruction >> 8)/* & 16777215*/;\
	
#else
#define I1OperandInt_Base()\
	uint32_t fop = (Instruction >> 8);\

#endif
	

// 2 operand memory instructions
// 0b00011111 get the repeat modifier
// 0b00011111 get the first operand
// 0b00000001 get last integer flag
// 0b00011111 get the second operand
#ifndef STAT_COUNTERS
#define I2OperandsMem_Base()\
	uint32_t rep = ((Instruction >> 8) & 31) + 1;\
	uint32_t fop = (Instruction >> 13) & 31;\
	uint32_t sop = (Instruction >> 18);\
	sop = pVM->Registers.r[sop];\

#else
	#define I2OperandsMem_Base()\
	uint32_t rep = ((Instruction >> 8) & 31) + 1;\
	uint32_t fop = (Instruction >> 13) & 31;\
	uint32_t sop = (Instruction >> 18) & 31;\
\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	pVM->RegistersHit[sop] = pVM->RegistersHit[sop] + 1;\
	sop = pVM->Registers.r[sop];\
	
#endif

#ifndef STAT_COUNTERS
#define I2OperandsMemInt_Base()\
	uint32_t rep = ((Instruction >> 8) & 31) + 1;\
	uint32_t fop = (Instruction >> 13) & 31;\
	uint32_t sop = (Instruction >> 18);\

#else
	#define I2OperandsMemInt_Base()\
	uint32_t rep = ((Instruction >> 8) & 31) + 1;\
	uint32_t fop = (Instruction >> 13) & 31;\
	uint32_t sop = (Instruction >> 18);\
\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	
#endif

#define I2OperandsMem_Check(type, size, memsize)\
	if(((type)sop + size * rep) > memsize){\
		assert(false);\
		return VM_DATA_ACCESS_VIOLATION;\
	}\


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
