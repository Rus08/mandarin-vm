
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