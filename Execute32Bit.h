
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
#define I3Operands_Base()\
\
	uint32_t rep = (Instruction >> 7) & 31; \
	uint32_t fop = (Instruction >> 12) & 31; \
	uint32_t last_int_flag = (Instruction >> 17) & 1; \
	uint32_t sop = (Instruction >> 18) & 31; \
	uint32_t top = (Instruction >> 23) & 511; 

// 2 operand instructions
// 0b00011111 get the repeat modifier
// 0b00011111 get the first operand
// 0b00000001 get last integer flag
// 0b00011111 get the second operand
#define I2Operands_Base()\
	uint32_t rep = (Instruction >> 7) & 31;\
	uint32_t fop = (Instruction >> 12) & 31;\
	uint32_t last_int_flag = (Instruction >> 17) & 1;\
	uint32_t sop = (Instruction >> 18) & 16383;


// 1 operand instructions
// 0b00000001 get last integer flag
#define I1Operand_Base()\
	uint32_t last_int_flag = (Instruction >> 7) & 1;\
	uint32_t fop;\
\
	if(last_int_flag == 0){\
			fop = pVM->Registers.r[(Instruction >> 8) & 31];\
		}else{\
			fop = (Instruction >> 8) & 16777215;\
		}
#define I1Operand_Checks()\
	if(((uint64_t)fop + 4) > pVM->CodeSize){\
		return VM_CODE_ACCESS_VIOLATION;\
	}

// 2 operand memory instructions
// 0b00011111 get the repeat modifier
// 0b00011111 get the first operand
// 0b00000001 get last integer flag
// 0b00011111 get the second operand
#define I2OperandsMem_Base()\
	uint32_t rep = (Instruction >> 7) & 31;\
	uint32_t fop = (Instruction >> 12) & 31;\
	uint32_t last_int_flag = (Instruction >> 17) & 1;\
	uint32_t sop;\
\
	if(last_int_flag == 0){\
		sop = pVM->Registers.r[(Instruction >> 18) & 31];\
	}else{\
		sop = (Instruction >> 18) & 16383;\
	}

