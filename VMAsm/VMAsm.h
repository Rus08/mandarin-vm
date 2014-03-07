
struct Label{
	char name[64];
	int string_num;
};

struct String{
	char* pBegin;
	char* pEnd;
	char* instr_name;
	char** op;
	int opnum;
	//int	opsize[3];
	int id;
	int	binary_size;
	int repeat;
	int offset;
};

enum SegmentType{
	SEG_CODE,
	SEG_DATA,
};

enum DataInstructionsId{
	VM_DWORD,
	VM_WORD,
	VM_BYTE,
	VM_INCLUDE,
};

struct Segment{
	int type;
	struct String* pStrings;
	int	StringsNum;
	struct Label* pLabels;
	int LabelsNum;
	char* pBinary;
};

int ClearComments(char* pAsm, int file_size);
int MakeStringsMap(char* pAsm, int file_size, struct Segment* pSeg);
int MakeLabelsMap(struct Segment* pSeg);
int ParseString(struct String* pString);
int DecodeOperand(char* Op, unsigned int IntMaxSize, int* pLastopintflag, bool signflag, int StringNum, struct Segment* pCodeSeg, struct Segment* pDataSeg, unsigned int* pOut);

int CalcSizeAndOffset(struct Segment* pSeg);
int CalcDataSizeAndOffset(struct Segment* pSeg, int StringOffset);
int ProcessCode(char* pSource, int code_size, struct Segment* pCodeSeg, struct Segment* pDataSeg);
int ProcessData(char* pSource, int code_size, struct Segment* pCodeSeg, struct Segment* pDataSeg, int StringOffset);