struct Instruction{
	char* name;
	int size;
	int	opnum;
	int intmaxsize;
	bool signflag;
	bool CodeSeg;
	bool DataSeg;
};

struct Instruction Instructions[] = {
//  name  size opnum max  sign  code  data
	"add",  4, 3, 16383, false, true, true,
	"sub",  4, 3, 16383, false, true, true,
	"mul",  4, 3, 16383, false, true, true,
	"div",  4, 3, 16383, true,  true, true,
	"mod",  4, 3, 16383, true,  true, true,
	"fadd", 4, 3, 16383, false, true, true,
	"fsub", 4, 3, 16383, false, true, true,
	"fmul", 4, 3, 16383, false, true, true,
	"fdiv", 4, 3, 16383, false, true, true,
	"shl",  4, 3, 16383, false, true, true,
	"shr",  4, 3, 16383, false, true, true,
	"and",  4, 3, 16383, false, true, true,
	"or",   4, 3, 16383, false, true, true,
	"xor",  4, 3, 16383, false, true, true,
/////////////////
	"cmp",  4, 2, 524287, false, true, true,
	"scmp", 4, 2, 524287, true,  true, true,
	"neg",  4, 2, 524287, false, true, true,
	"fcmp", 4, 2, 524287, true,  true, true,
	"ftoi", 4, 2, 524287, false, true, true,
	"itof", 4, 2, 524287, true,  true, true,
	"not",  4, 2, 524287, false, true, true,
/////////////////
	"call", 4, 1, 16777215, false, true, false,
	"jeq",  4, 1, 16777215, false, true, false,
	"jne",  4, 1, 16777215, false, true, false,
	"jgr",  4, 1, 16777215, false, true, false,
	"jls",  4, 1, 16777215, false, true, false,
	"jge",  4, 1, 16777215, false, true, false,
	"jle",  4, 1, 16777215, false, true, false,
	"jmp",  4, 1, 16777215, false, true, false,
/////////////////
	"load", 4, 2, 16383, false, false, true,
	"loadw", 4, 2, 16383, false, false, true,
	"loadb", 4, 2, 16383, false, false, true,
	"store", 4, 2, 16383, false, false, true,
	"storew", 4, 2, 16383, false, false, true,
	"storeb", 4, 2, 16383, false, false, true,
// local
	"loadl", 4, 2, 16383, false, false, true,
	"loadlw", 4, 2, 16383, false, false, true,
	"loadlb", 4, 2, 16383, false, false, true,
	"storel", 4, 2, 16383, false, false, true,
	"storelw", 4, 2, 16383, false, false, true,
	"storelb", 4, 2, 16383, false, false, true,
/////////////////
	"ldi",  4, 2, 524287, false, true, true,
	"mov",  4, 2, 0, false, true, true,
	"copy", 4, 2, 0, false, true, true,
	"syscall", 4, 1, 16777215, false, false, false,
	"ret",  4, 0, 0, false, true, true,
///////////////////
	"memcpy", 4, 3, 511, false, false, true,
};

struct Instruction Instructions16Bit[] = {
	"hadd", 2, 3, 0, false, true, true,
	"hsub", 2, 3, 0, false, true, true,
	"hand", 2, 3, 0, false, true, true,
	"hor", 2, 3, 0, false, true, true,
	"hfadd", 2, 3, 0, false, true, true,
	"hmul", 2, 3, 0, false, true, true,
	"hfmul", 2, 3, 0, false, true, true,
	"hnop", 2, 0, 0, false, true, true,
};

struct Instruction DataInstructions[] = {
	"dword", 4, 0, 0xffffffff, false, true, true,
	"word", 2, 0, 0xffff, false, true, true,
	"byte", 1, 0, 0xff, false, true, true,
	"include", 0, 0, 0, false, true, true,
};