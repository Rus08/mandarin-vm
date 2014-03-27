#define INT_FLAG 1

#define I3OPENUM(arg)\
	arg,\
	arg##I,\
	arg##V

#define IMEMENUM(arg)\
	arg,\
	arg##I,\
	arg##V,\
	arg##VI

enum InstructionsId{
	I3OPENUM(VM_ADD),
	I3OPENUM(VM_SUB),
	I3OPENUM(VM_MUL),
	I3OPENUM(VM_DIV),
	I3OPENUM(VM_SDIV),
	I3OPENUM(VM_MOD),
	I3OPENUM(VM_SMOD),
	I3OPENUM(VM_FADD),
	I3OPENUM(VM_FSUB),
	I3OPENUM(VM_FMUL),
	I3OPENUM(VM_FDIV),
	I3OPENUM(VM_SHL),
	I3OPENUM(VM_SHR),
	I3OPENUM(VM_SAR),
	I3OPENUM(VM_AND),
	I3OPENUM(VM_OR),
	I3OPENUM(VM_XOR),
///////////////////
	I3OPENUM(VM_CMP),
	I3OPENUM(VM_SCMP),
	I3OPENUM(VM_NEG),
	I3OPENUM(VM_FCMP),
	I3OPENUM(VM_FTOI),
	I3OPENUM(VM_ITOF),
	I3OPENUM(VM_NOT),
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
	IMEMENUM(VM_LOAD),
	IMEMENUM(VM_LOADW),
	IMEMENUM(VM_LOADB),
	IMEMENUM(VM_STORE),
	IMEMENUM(VM_STOREW),
	IMEMENUM(VM_STOREB),
// local
	IMEMENUM(VM_LOADL),
	IMEMENUM(VM_LOADLW),
	IMEMENUM(VM_LOADLB),
	IMEMENUM(VM_STOREL),
	IMEMENUM(VM_STORELW),
	IMEMENUM(VM_STORELB),
///////////////////
	VM_LDI,
	VM_MOV,
	VM_MOVV,
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

#define IDefaultEnd()\
	pVM->Registers.PC = pVM->Registers.PC + 4;

#define I3Operands(id, type, op, INTprefix, divcheck, intunpack)\
	case id:\
	{\
		I3Operands_Base();\
		/* third operand is register*/\
		if(divcheck != false && pVM->Registers.r[top] == 0){\
			assert(false);\
			return VM_DIVIDE_BY_ZERO;\
		}\
		*(type*)&pVM->Registers.r[fop] = *(type*)&pVM->Registers.r[sop] op *(type*)&pVM->Registers.r[top];\
		IDefaultEnd();\
	}\
	break;\
	case id##I:\
	{\
		I3OperandsInt_Base();\
		/* third operand is integer*/\
		if(intunpack != 0){\
			top = top - intunpack;\
		}\
		*(type*)&pVM->Registers.r[fop] = *(type*)&pVM->Registers.r[sop] op INTprefix##top;\
		IDefaultEnd();\
	}\
	break;\
	case id##V:\
	{\
		I3Operands_Base();\
		/* third operand is register*/\
		for(uint32_t i = 0; i <= rep; i++){\
			if(divcheck != false && pVM->Registers.r[top + i] == 0){\
				assert(false);\
				return VM_DIVIDE_BY_ZERO;\
			}\
			*(type*)&pVM->Registers.r[fop + i] = *(type*)&pVM->Registers.r[sop + i] op *(type*)&pVM->Registers.r[top + i];\
		}\
		IDefaultEnd();\
	}\
	break;\
	

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

#define I2Operands(id, type, op, intunpack)\
	case id:\
	{\
		I2Operands_Base();\
		/* second operand is register*/\
		*(type*)&pVM->Registers.r[fop] = op##pVM->Registers.r[sop];\
		IDefaultEnd();\
	}\
	break;\
	case id##I:\
	{\
		I2OperandsInt_Base();\
		/* second operand is integer*/\
		if(intunpack != 0){\
			sop = sop - intunpack;\
		}\
		*(type*)&pVM->Registers.r[fop] = op##sop;\
		IDefaultEnd();\
	}\
	break;\
	case id##V:\
	{\
		I2Operands_Base();\
		/* second operand is register*/\
		for(uint32_t i = 0; i <= rep; i++){\
			*(type*)&pVM->Registers.r[fop + i] = op##pVM->Registers.r[sop + i];\
		}\
		IDefaultEnd();\
	}\
	break;\

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

#define I2OperandsMem_Check(type, size, rep, memsize)\
	if(((type)sop + size * rep) > memsize){\
		assert(false);\
		return VM_DATA_ACCESS_VIOLATION;\
	}\

#define I2OperandsMem(id, type, pointer, memsize, load)\
	case id:\
	{\
		I2OperandsMem_Base();\
		/* second operand is register*/\
		I2OperandsMem_Check(uint64_t, sizeof(type), 1, memsize);\
\
		if(load == true){\
			pVM->Registers.r[fop] = *(type*)(pointer + sop);\
		}else{\
			*(type*)(pointer + sop) = pVM->Registers.r[fop];\
		}\
		IDefaultEnd();\
	}\
	break;\
	case id##I:\
	{\
		I2OperandsMemInt_Base();\
		/* second operand is integer*/\
		I2OperandsMem_Check(uint32_t, sizeof(type), 1, memsize);\
\
		if(load == true){\
			pVM->Registers.r[fop] = *(type*)(pointer + sop);\
		}else{\
			*(type*)(pointer + sop) = pVM->Registers.r[fop];\
		}\
		IDefaultEnd();\
	}\
	break;\
	case id##V:\
	{\
		I2OperandsMem_Base();\
		/* second operand is register*/\
		I2OperandsMem_Check(uint64_t, sizeof(type), rep, memsize);\
\
		for(uint32_t i = 0; i < rep; i++){\
			if(load == true){\
				pVM->Registers.r[fop + i] = *(type*)(pointer + sop + sizeof(type) * i);\
			}else{\
				*(type*)(pointer + sop + sizeof(type) * i) = pVM->Registers.r[fop + i];\
			}\
		}\
		IDefaultEnd();\
	}\
	break;\
	case id##VI:\
	{\
		I2OperandsMemInt_Base();\
		/* second operand is integer*/\
		I2OperandsMem_Check(uint32_t, sizeof(type), rep, memsize);\
\
		for(uint32_t i = 0; i < rep; i++){\
			if(load == true){\
				pVM->Registers.r[fop + i] = *(type*)(pointer + sop + sizeof(type) * i);\
			}else{\
				*(type*)(pointer + sop + sizeof(type) * i) = pVM->Registers.r[fop + i];\
			}\
		}\
		IDefaultEnd();\
	}\
	break;\

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
		struct Call* pCallStack = (Call*)realloc(pVM->pCallStack, sizeof(Call) * pVM->CallStackSize);\
		if(pCallStack == NULL){\
			assert(false);\
			return VM_NOT_ENOUGH_MEMORY;\
		}\
		pVM->pCallStack = pCallStack;\
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
