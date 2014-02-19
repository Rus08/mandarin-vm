
struct Label{
	char name[64];
	int string_num;
};

struct String{
	char* pBegin;
	char* pEnd;
	char* instr_name;
	//int instr_namesize;
	char* op[3];
	//int	opsize[3];
	int id;
	int	instruction_size;
	int repeat;
	int offset;
};

int ClearComments(char* pAsm, int file_size);
int MakeStringsMap(char* pAsm, int file_size);
int MakeLabelsMap(char* pAsm, int file_size);
int CalcSizeAndOffset(struct String* pString);
int CodeInstruction(struct String* pString, struct Label* pLabels, char* pOutBuf);

int ParseString(struct String* pString);