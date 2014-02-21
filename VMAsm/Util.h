

int GetId(char* name);
int GetSize(int id);
int GetOperandCount(int id);
int GetIntMaxSize(int id);
unsigned int GetLabelAddr(char* name);
unsigned int GetSysCallAddr(char* name);
bool IsNumber(char* op);
int CheckForHiLo(char* op);
int DecodeHiLo(char* op, unsigned int* pMask, int* pShift);