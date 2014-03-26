

int GetId(char* name);
int GetCodeId(int id, int formnum);
int GetDataId(char* name);
int GetSize(int id);
int GetDataSize(int id);
int GetOperandCount(int id);
int GetIntMaxSize(int id);
bool IfSigned(int id);
bool IfCodeSeg(int id);
bool IfDataSeg(int id);
int GetFileSize(char* path);
unsigned int GetLabelAddr(char* name, struct Segment* pSeg);
unsigned int GetSysCallAddr(char* name);
bool IsDataInstr(char* string);
bool IsNumber(char* op);
int CheckForHiLo(char* op);
int DecodeHiLo(char* op, unsigned int* pMask, int* pShift);
unsigned int DecodeHex(char* Op);
int DecodeBit(char* Op);