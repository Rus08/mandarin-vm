
#define I3OPENUM(arg)\
	arg,\
	arg##I,\
	arg##V,\
	arg##VS

#define I3OPFENUM(arg)\
	arg,\
	arg##V,\
	arg##VS

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
	I3OPFENUM(VM_FADD),
	I3OPFENUM(VM_FSUB),
	I3OPFENUM(VM_FMUL),
	I3OPFENUM(VM_FDIV),
	I3OPENUM(VM_SHL),
	I3OPENUM(VM_SHR),
	I3OPENUM(VM_SAR),
	I3OPENUM(VM_AND),
	I3OPENUM(VM_OR),
	I3OPENUM(VM_XOR),
///////////////////
	I3OPENUM(VM_CMP_EQ),
	I3OPENUM(VM_CMP_NEQ),
	I3OPENUM(VM_CMP_GR),
	I3OPENUM(VM_CMP_LS),
	I3OPENUM(VM_CMP_GRE),
	I3OPENUM(VM_CMP_LSE),
	I3OPENUM(VM_SCMP_GR),
	I3OPENUM(VM_SCMP_LS),
	I3OPENUM(VM_SCMP_GRE),
	I3OPENUM(VM_SCMP_LSE),
	VM_NEG,
	VM_NEGV,
	VM_LDI,
	VM_LDIVS,
	I3OPFENUM(VM_FCMP_EQ),
	I3OPFENUM(VM_FCMP_NEQ),
	I3OPFENUM(VM_FCMP_GR),
	I3OPFENUM(VM_FCMP_LS),
	I3OPFENUM(VM_FCMP_GRE),
	I3OPFENUM(VM_FCMP_LSE),
	I3OPFENUM(VM_FTOI),
	I3OPENUM(VM_ITOF),
	VM_NOT,
	VM_NOTV,
	VM_MOV,
	VM_MOVV,
	VM_MOVVS,
///////////////////
	VM_CALL,
	VM_CALLI,
	VM_RET,
	VM_JEQ,
	VM_JEQI,
	VM_JNE,
	VM_JNEI,
	I3OPENUM(VM_JEQR),
	I3OPENUM(VM_JNER),
	VM_JMP,
	VM_JMPI,
	VM_SYSCALL,
///////////////////
	IMEMENUM(VM_LOAD),
	IMEMENUM(VM_LOADW),
	IMEMENUM(VM_LOADB),
	IMEMENUM(VM_STORE),
	IMEMENUM(VM_STOREW),
	IMEMENUM(VM_STOREB),
////////////////////
	VM_MEMCPY,
	VM_MEMCPYI	
};

//uint32_t Execute32Bit(struct VirtualMachine* pVM, uint32_t Instruction);


// 3 operand instructions
// this is for 3 operand register-register
#ifndef STAT_COUNTERS
#define I3Operands_Base()\
\
	uint32_t fop = (Instruction >> 8) & 31; \
	uint32_t sop = (Instruction >> 13) & 31; \
	uint32_t top = (Instruction >> 27); 
#else
	#define I3Operands_Base()\
\
	uint32_t fop = (Instruction >> 8) & 31; \
	uint32_t sop = (Instruction >> 13) & 31; \
	uint32_t top = (Instruction >> 27); \
	\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	pVM->RegistersHit[sop] = pVM->RegistersHit[sop] + 1;\
	if((id & 1) == 0){\
		pVM->RegistersHit[top] = pVM->RegistersHit[top] + 1;\
	}\

#endif

// this is for 3 operand register-integer forms
#ifndef STAT_COUNTERS
#define I3OperandsI_Base()\
\
	uint32_t fop = (Instruction >> 8) & 31; \
	uint32_t sop = (Instruction >> 13) & 31; \
	uint32_t top = (Instruction >> 18)/* & 16383*/; 
#else
	#define I3OperandsI_Base()\
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

// this is for 3 operand register-register vector forms
#ifndef STAT_COUNTERS
#define I3OperandsV_Base()\
\
	uint32_t rep = (Instruction >> 8) & 31; \
	uint32_t fop = (Instruction >> 13) & 31; \
	uint32_t sop = (Instruction >> 18) & 31; \
	uint32_t top = (Instruction >> 27)/* & 511*/; 
#else
	#define I3Operands_Base()\
\
	uint32_t rep = (Instruction >> 8) & 31; \
	uint32_t fop = (Instruction >> 13) & 31; \
	uint32_t sop = (Instruction >> 18) & 31; \
	uint32_t top = (Instruction >> 27); \
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
		I3OperandsI_Base();\
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
		I3OperandsV_Base();\
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
	case id##VS:\
	{\
		I3OperandsV_Base();\
		/* third operand is register*/\
		for(uint32_t i = 0; i <= rep; i++){\
			if(divcheck != false && pVM->Registers.r[top + i] == 0){\
				assert(false);\
				return VM_DIVIDE_BY_ZERO;\
			}\
			*(type*)&pVM->Registers.r[fop + i] = *(type*)&pVM->Registers.r[sop + i] op *(type*)&pVM->Registers.r[top];\
		}\
		IDefaultEnd();\
	}\
	break;\

#define I3OperandsF(id, type, op, INTprefix, divcheck, intunpack)\
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
	case id##V:\
	{\
		I3OperandsV_Base();\
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
	case id##VS:\
	{\
		I3OperandsV_Base();\
		/* third operand is register*/\
		for(uint32_t i = 0; i <= rep; i++){\
			if(divcheck != false && pVM->Registers.r[top + i] == 0){\
				assert(false);\
				return VM_DIVIDE_BY_ZERO;\
			}\
			*(type*)&pVM->Registers.r[fop + i] = *(type*)&pVM->Registers.r[sop + i] op *(type*)&pVM->Registers.r[top];\
		}\
		IDefaultEnd();\
	}\
	break;\

// 2 operand instructions
// this is for 2 operand register-register
#ifndef STAT_COUNTERS
#define I2Operands_Base()\
	uint32_t fop = (Instruction >> 8) & 31;\
	uint32_t sop = (Instruction >> 27);
#else
#define I2Operands_Base()\
	uint32_t fop = (Instruction >> 8) & 31;\
	uint32_t sop = (Instruction >> 13);\
	\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	pVM->RegistersHit[sop] = pVM->RegistersHit[sop] + 1;\
	
#endif

// this is for register-integer form
#ifndef STAT_COUNTERS
#define I2OperandsI_Base()\
	uint32_t fop = (Instruction >> 8) & 31;\
	uint32_t sop = (Instruction >> 13)/* & 524287*/;
#else
#define I2OperandsI_Base()\
	uint32_t fop = (Instruction >> 8) & 31;\
	uint32_t sop = (Instruction >> 13);\
	\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	pVM->RegistersHit[sop] = pVM->RegistersHit[sop] + 1;\
	
#endif

// this is for 2 operand register-register vector forms
#ifndef STAT_COUNTERS
#define I2OperandsV_Base()\
	uint32_t rep = (Instruction >> 8) & 31;\
	uint32_t fop = (Instruction >> 13) & 31;\
	uint32_t sop = (Instruction >> 27)/* & 31*/;
#else
#define I2Operands_Base()\
	uint32_t rep = (Instruction >> 8) & 31;\
	uint32_t fop = (Instruction >> 13) & 31;\
	uint32_t sop = (Instruction >> 27);\
	\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;\
	pVM->RegistersHit[sop] = pVM->RegistersHit[sop] + 1;\
	
#endif

// cmp instructions
#define I2OperandsCMP(id, type, op, INTprefix, intunpack)\
	case id:\
	{\
		I2Operands_Base();\
		/* second operand is register*/\
		*(uint8_t*)&pVM->Registers.REQ = *(type*)&pVM->Registers.r[fop] op *(type*)&pVM->Registers.r[sop];\
		IDefaultEnd();\
	}\
	break;\
	case id##I:\
	{\
		I2OperandsI_Base();\
		/* second operand is integer*/\
		if(intunpack != 0){\
			sop = sop - intunpack;\
		}\
		*(uint8_t*)&pVM->Registers.REQ = *(type*)&pVM->Registers.r[fop] op INTprefix##sop;\
		IDefaultEnd();\
	}\
	break;\
	case id##V:\
	{\
		I3OperandsV_Base();\
		/* vector form operands are registers*/\
		for(uint32_t i = 0; i <= rep; i++){\
			*(type*)&pVM->Registers.r[fop + i] = *(type*)&pVM->Registers.r[sop + i] op *(type*)&pVM->Registers.r[top + i];\
		}\
		IDefaultEnd();\
	}\
	break;\
	case id##VS:\
	{\
		I3OperandsV_Base();\
		/* vector form operands are registers*/\
		for(uint32_t i = 0; i <= rep; i++){\
			*(type*)&pVM->Registers.r[fop + i] = *(type*)&pVM->Registers.r[sop + i] op *(type*)&pVM->Registers.r[top];\
		}\
		IDefaultEnd();\
	}\
	break;\


#define I2OperandsFCMP(id, type, op)\
	case id:\
	{\
		I2Operands_Base();\
		/* second operand is register*/\
		*(uint8_t*)&pVM->Registers.REQ = *(type*)&pVM->Registers.r[fop] op *(type*)&pVM->Registers.r[sop];\
		IDefaultEnd();\
	}\
	break;\
	case id##V:\
	{\
		I3OperandsV_Base();\
		/* vector form operands are registers*/\
		for(uint32_t i = 0; i <= rep; i++){\
			*(type*)&pVM->Registers.r[fop + i] = *(type*)&pVM->Registers.r[sop + i] op *(type*)&pVM->Registers.r[top + i];\
		}\
		IDefaultEnd();\
	}\
	break;\
	case id##VS:\
	{\
		I3OperandsV_Base();\
		/* vector form operands are registers*/\
		for(uint32_t i = 0; i <= rep; i++){\
			*(type*)&pVM->Registers.r[fop + i] = *(type*)&pVM->Registers.r[sop + i] op *(type*)&pVM->Registers.r[top];\
		}\
		IDefaultEnd();\
	}\
	break;\


// 1 operand instructions
#ifndef STAT_COUNTERS
#define I1Operand_Base()\
	uint32_t fop = Instruction >> 27;\
	
#else
#define I1Operand_Base()\
	uint32_t fop = (Instruction >> 27);\
\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;
#endif

#ifndef STAT_COUNTERS
#define I1OperandI_Base()\
	uint32_t fop = Instruction >> 8;\
	
#else
#define I1OperandI_Base()\
	uint32_t fop = (Instruction >> 8);\
\
	pVM->RegistersHit[fop] = pVM->RegistersHit[fop] + 1;
#endif
	
#define CodeAddressCheck(arg)\
	if(((uint64_t)(arg) + 4) > pVM->CodeSize){\
		assert(false);\
		return VM_CODE_ACCESS_VIOLATION;\
	}

#define I2OperandsMem_Check(type, size, rep, memsize)\
	if(((type)sop + size * rep) > memsize){\
		assert(false);\
		return VM_DATA_ACCESS_VIOLATION;\
	}\

#define I2OperandsMem(id, type, pointer, memsize, load)\
	case id:\
	{\
		I2Operands_Base();\
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
		I2OperandsI_Base();\
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
		I2OperandsV_Base();\
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
		I2OperandsV_Base();\
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

#define CheckStackSize(){\
	if(pVM->CurrentStackTop == pVM->CallStackSize){\
		pVM->CallStackSize = pVM->CallStackSize + 32;\
		if(pVM->CallStackSize >= MAX_ALLOWED_STACK_SIZE){\
			return VM_STACK_IS_TOO_BIG;\
		}\
		struct Call* pCallStack = (Call*)vm_realloc(pVM->pCallStack, sizeof(Call) * pVM->CallStackSize);\
		if(pCallStack == NULL){\
			assert(false);\
			return VM_NOT_ENOUGH_MEMORY;\
		}\
		pVM->pCallStack = pCallStack;\
	}\
}

#define MakeCall(){\
	pVM->pCallStack[pVM->CurrentStackTop].regPC = pVM->Registers.PC + 4; /* next instruction after call*/ \
}
