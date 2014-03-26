struct Instruction{
	char* name;
	int size;
	int	opnum;
	int intmaxsize;
	bool signflag;
	bool CodeSeg;
	bool DataSeg;
	bool rrform[4]; // 0 reg reg, 1 - reg int, 2 - reg reg vect, 3 - reg int vect
};

struct Instruction Instructions[] = {
//  name  size opnum max  sign  code  data  r r   r i   r v   r v i
	"add",  4, 3, 16383, false, true, true, true, true, true, false,
	"sub",  4, 3, 16383, false, true, true, true, true, true, false,
	"mul",  4, 3, 16383, false, true, true, true, true, true, false,
	"div",  4, 3, 16383, false, true, true, true, true, true, false,
	"sdiv", 4, 3, 16383, true,  true, true, true, true, true, false,
	"mod",  4, 3, 16383, false, true, true, true, true, true, false,
	"smod", 4, 3, 16383, true,  true, true, true, true, true, false,
	"fadd", 4, 3, 16383, true, true, true, true, true, true, false,
	"fsub", 4, 3, 16383, true, true, true, true, true, true, false,
	"fmul", 4, 3, 16383, true, true, true, true, true, true, false,
	"fdiv", 4, 3, 16383, true, true, true, true, true, true, false,
	"shl",  4, 3, 16383, false, true, true, true, true, true, false,
	"shr",  4, 3, 16383, false, true, true, true, true, true, false,
	"sar",  4, 3, 16383, false, true, true, true, true, true, false,
	"and",  4, 3, 16383, false, true, true, true, true, true, false,
	"or",   4, 3, 16383, false, true, true, true, true, true, false,
	"xor",  4, 3, 16383, false, true, true, true, true, true, false,
/////////////////
	"cmp",  4, 2, 524287, false, true, true, true, true, true, false,
	"scmp", 4, 2, 524287, true,  true, true, true, true, true, false,
	"neg",  4, 2, 524287, false, true, true, true, true, true, false,
	"fcmp", 4, 2, 524287, true,  true, true, true, true, true, false,
	"ftoi", 4, 2, 524287, false, true, true, true, true, true, false,
	"itof", 4, 2, 524287, true,  true, true, true, true, true, false,
	"not",  4, 2, 524287, false, true, true, true, true, true, false,
/////////////////
	"call", 4, 1, 16777215, false, true, false, true, true, false, false,
	"jeq",  4, 1, 16777215, false, true, false, true, true, false, false,
	"jne",  4, 1, 16777215, false, true, false, true, true, false, false,
	"jgr",  4, 1, 16777215, false, true, false, true, true, false, false,
	"jls",  4, 1, 16777215, false, true, false, true, true, false, false,
	"jge",  4, 1, 16777215, false, true, false, true, true, false, false,
	"jle",  4, 1, 16777215, false, true, false, true, true, false, false,
	"jmp",  4, 1, 16777215, false, true, false, true, true, false, false,
/////////////////
	"load", 4, 2, 16383, false, false, true,    true, true, true, true,
	"loadw", 4, 2, 16383, false, false, true,   true, true, true, true,
	"loadb", 4, 2, 16383, false, false, true,   true, true, true, true,
	"store", 4, 2, 16383, false, false, true,   true, true, true, true,
	"storew", 4, 2, 16383, false, false, true,  true, true, true, true,
	"storeb", 4, 2, 16383, false, false, true,  true, true, true, true,
// local
	"loadl", 4, 2, 16383, false, false, true,   true, true, true, true,
	"loadlw", 4, 2, 16383, false, false, true,  true, true, true, true,
	"loadlb", 4, 2, 16383, false, false, true,  true, true, true, true,
	"storel", 4, 2, 16383, false, false, true,  true, true, true, true,
	"storelw", 4, 2, 16383, false, false, true, true, true, true, true,
	"storelb", 4, 2, 16383, false, false, true, true, true, true, true,
/////////////////
	"ldi",  4, 2, 524287, false, true, true,	false, true, false, false,
	"mov",  4, 2, 0, false, true, true,			true, false, true, false,
	"copy", 4, 2, 0, false, true, true,			false, false, true, false,
	"syscall", 4, 1, 16777215, false, false, false, false, true, false, false,
	"ret",  4, 0, 0, false, true, true,         true, false, false, false,
///////////////////
	"memcpy", 4, 3, 511, false, false, true,    true, false, false, false,
};


struct Instruction DataInstructions[] = {
	"dword", 4, 0, 0xffffffff, false, true, true, false, false, false, false,
	"word", 2, 0, 0xffff, false, true, true,	  false, false, false, false,
	"byte", 1, 0, 0xff, false, true, true,		  false, false, false, false,
	"include", 0, 0, 0, false, true, true,        false, false, false, false,
};