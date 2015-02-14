struct Instruction{
	char* name;
	int size;
	int	opnum;
	int intmaxsize;
	bool signflag;
	bool CodeSeg;
	bool DataSeg;
	bool lastopint;
	bool repeat;
	bool rep_single;
};

#define I3DEF(name, sign)\
	name, 4, 3, 16383, sign, true, true, false, false, false,\
	name##"i", 4, 3, 16383, sign, true, true, true, false, false,\
	name##"v", 4, 3, 16383, sign, true, true, false, true, false,\
	name##"vs", 4, 3, 16383, sign, true, true, false, true, true

#define I3FDEF(name)\
	name, 4, 3, 16383, true, true, true, false, false, false,\
	name##"v", 4, 3, 16383, true, true, true, false, true, false,\
	name##"vs", 4, 3, 16383, true, true, true, false, true, true

#define I3CMPDEF(name, sign)\
	name, 4, 2, 524287, sign, true, true, false, false, false,\
	name##"i", 4, 2, 524287, sign, true, true, true, false, false,\
	name##"v", 4, 3, 16383, sign, true, true, false, true, false,\
	name##"vs", 4, 3, 16383, sign, true, true, false, true, true

#define I3FCMPDEF(name)\
	name, 4, 2, 524287, true, true, true, false, false, false,\
	name##"v", 4, 3, 16383, true, true, true, false, true, false,\
	name##"vs", 4, 3, 16383, true, true, true, false, true, true

#define I2MEMDEF(name)\
	name, 4, 2, 524287, false, false, true, false, false, false,\
	name##"i", 4, 2, 524287, false, false, true, true, false, false,\
	name##"v", 4, 2, 16383, false, false, true, false, true, false,\
	name##"vi", 4, 2, 16383, false, false, true, true, true, false

struct Instruction Instructions[] = {
//  name  size opnum max  sign  code  data  r r   r i   r v   r v i
	I3DEF("add", false),
	I3DEF("sub",  false),
	I3DEF("mul",  false),
	I3DEF("div",  false),
	I3DEF("sdiv", true),
	I3DEF("mod",  false),
	I3DEF("smod", true),
	I3FDEF("fadd"),
	I3FDEF("fsub"),
	I3FDEF("fmul"),
	I3FDEF("fdiv"),
	I3DEF("shl",  false),
	I3DEF("shr",  false),
	I3DEF("sar",  false),
	I3DEF("and",  false),
	I3DEF("or",   false),
	I3DEF("xor",  false),
/////////////////
	I3CMPDEF("cmp_eq", false),
	I3CMPDEF("cmp_neq", false),
	I3CMPDEF("cmp_gr", false),
	I3CMPDEF("cmp_ls", false),
	I3CMPDEF("cmp_gre", false),
	I3CMPDEF("cmp_lse", false),
	I3CMPDEF("scmp_gr", true),
	I3CMPDEF("scmp_ls", true),
	I3CMPDEF("scmp_gre", true),
	I3CMPDEF("scmp_lse", true),
	"neg",  4, 2, 524287, false, true, true, false, false, false,
	"negv", 4, 2, 16383, false, true, true, false, true, false,
	"ldi",  4, 2, 524287, false, true, true,	true, false, false,
	"ldivs",4, 2, 16383, false, true, true, true, true, true,
	I3FCMPDEF("fcmp_eq"),
	I3FCMPDEF("fcmp_neq"),
	I3FCMPDEF("fcmp_gr"),
	I3FCMPDEF("fcmp_ls"),
	I3FCMPDEF("fcmp_gre"),
	I3FCMPDEF("fcmp_lse"),
	I3FDEF("ftoi"),
	I3DEF("itof", true),
	"not",  4, 2, 524287, false, true, true, false, false, false,
	"notv", 4, 2, 16383, false, true, true, false, true, false,
	"mov",  4, 2, 524287, false, true, true, false, false, false,
	"movv", 4, 2, 16383,  false, true, true, false, true, false,
	"movvs",4, 2, 16383,  false, true, true, false, true, true,
/////////////////
	"call", 4, 1, 16777215, false, true, false, false, false, false,
	"calli",4, 1, 16777215, true, true, false, true, false, false,
	"ret",  4, 0, 0, false, true, true,         false, false, false,
	"jeq",  4, 1, 16777215, false, true, false, false, false, false,
	"jeqi", 4, 1, 16777215, true, true, false, true, false, false,
	"jne",  4, 1, 16777215, false, true, false, false, false, false,
	"jnei", 4, 1, 16777215, true, true, false, true, false, false,
	"jeqr",   4, 2, 524287, false, true, false, false, false, false,
	"jeqri",  4, 2, 524287, true, true, false, true, false, false,
	"jeqrv",  4, 2, 524287, false, true, false, false, true, false,
	"jeqrvs", 4, 2, 524287, false, true, false, false, true, true,
	"jner",   4, 2, 524287, false, true, false, false, false, false,
	"jneri",  4, 2, 524287, true, true, false, true, false, false,
	"jnerv",  4, 2, 524287, false, true, false, false, true, false,
	"jnervs", 4, 2, 524287, false, true, false, false, true, true,
	"jmp",  4, 1, 16777215, false, true, false, false, false, false,
	"jmpi", 4, 1, 16777215, true, true, false, true, false, false,
/////////////////
	"syscall", 4, 1, 16777215, false, false, false, true, false, false,
/////////////////
	I2MEMDEF("load"),
	I2MEMDEF("loadw"),
	I2MEMDEF("loadb"),
	I2MEMDEF("store"),
	I2MEMDEF("storew"),
	I2MEMDEF("storeb"),
///////////////////
	"memcpy", 4, 3, 16383, false, false, true, false, false, false,
	"memcpyi", 4, 3, 16383, false, false, true, true, false, false,
};


struct Instruction DataInstructions[] = {
	"dword", 4, 0, 0xffffffff, false, true, true, false, false, false,
	"word", 2, 0, 0xffff, false, true, true,	  false, false, false,
	"byte", 1, 0, 0xff, false, true, true,		  false, false, false,
	"include", 0, 0, 0, false, true, true,        false, false, false,
};