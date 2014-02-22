

int GetId(char* name);
int GetDataId(char* name);
int GetSize(int id);
int GetDataSize(int id);
int GetFileSize(char* path);
int GetOperandCount(int id);
int GetIntMaxSize(int id);
unsigned int GetLabelAddr(char* name, struct Segment* pSeg);
unsigned int GetSysCallAddr(char* name);
bool IsDataInstr(char* string);
bool IsNumber(char* op);
int CheckForHiLo(char* op);
int DecodeHiLo(char* op, unsigned int* pMask, int* pShift);
int DecodeHex(char* Op);
int DecodeBit(char* Op);