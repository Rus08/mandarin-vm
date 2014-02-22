struct Instruction{
	char* name;
	int size;
	int	opnum;
	int intmaxsize;
};

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
	VM_NEG,
	VM_LDI,
	VM_FCMP,
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
	
};

struct Instruction Instructions[] = {
	"add",  4, 3, 511,
	"sub",  4, 3, 511,
	"mul",  4, 3, 511,
	"div",  4, 3, 511,
	"mod",  4, 3, 511,
	"fadd", 4, 3, 511,
	"fsub", 4, 3, 511,
	"fmul", 4, 3, 511,
	"fdiv", 4, 3, 511,
	"shl",  4, 3, 511,
	"shr",  4, 3, 511,
	"and",  4, 3, 511,
	"or",   4, 3, 511,
	"xor",  4, 3, 511,
/////////////////
	"cmp",  4, 2, 16383,
	"neg",  4, 2, 16383,
	"ldi",  4, 2, 16383,
	"fcmp", 4, 2, 16383,
	"not",  4, 2, 16383,
/////////////////
	"call", 4, 1, 16777215,
	"jeq",  4, 1, 16777215,
	"jne",  4, 1, 16777215,
	"jgr",  4, 1, 16777215,
	"jls",  4, 1, 16777215,
	"jge",  4, 1, 16777215,
	"jle",  4, 1, 16777215,
	"jmp",  4, 1, 16777215,
	"syscall", 4, 1, 16777215,
/////////////////
	"ret",  4, 0, 0,
/////////////////
	"load", 4, 2, 16383,
	"loadl", 4, 2, 16383,
	"loadw", 4, 2, 16383,
	"loadlw", 4, 2, 16383,
	"loadb", 4, 2, 16383,
	"loadlb", 4, 2, 16383,
	"store", 4, 2, 16383,
	"storel", 4, 2, 16383,
	"storew", 4, 2, 16383,
	"storelw", 4, 2, 16383,
	"storeb", 4, 2, 16383,
	"storelb", 4, 2, 16383,
///////////////////
	
};

struct Instruction DataInstructions[] = {
	"dword", 4, 0, 0xffffffff,
	"word", 2, 0, 0xffff,
	"byte", 1, 0, 0xff,
	"include", 0, 0, 0,
};