#include <stdlib.h>
#include <string.h>
//#include <stdbool.h>
#include "VMAsm.h"
#include "Util.h"
#include "Instructions.h"
#include "SysCalls.h"

extern struct String* pStrings;
extern struct Label* pLabels;

extern int StringsNum;
extern int LabelsNum;

char* hilotable[8] = {	"hi32", "hi24", "hi16", "hi8", "lo32", "lo24", "lo16", "lo8"};
unsigned int masktable[8] = { 0xffffffff, 0xffffff00, 0xffff0000, 0x0000ff00, 0xffffffff, 0x00ffffff, 0x0000ffff, 0x000000ff };
int shifttable[8] = { 32, 24, 16, 8, 0, 0, 0, 0 };

int GetId(char* name)
{
	int id = -1;

	for(int b = 0; b < sizeof(Instructions) / sizeof(struct Instruction); b++){
		if(strncmp(name, Instructions[b].name, strlen(name)/*strlen(Instructions[b].name)*/) == 0){
			id = b;
			break;
		}
	}
	return id;
}

int GetSize(int id)
{
	return Instructions[id].size;
}

int GetOperandCount(int id)
{
	return Instructions[id].opnum;
}

int GetIntMaxSize(int id)
{
	return Instructions[id].intmaxsize;
}

unsigned int GetLabelAddr(char* name)
{
	unsigned int addr = -1;

	for(int i = 0; i < LabelsNum; i++){
		if(strncmp(pLabels[i].name, name, strlen(pLabels[i].name)) == 0){
			return pStrings[pLabels[i].string_num].offset;
		}
	}

	return addr;
}

unsigned int GetSysCallAddr(char* name)
{
	unsigned int addr = -1;

	for(int i = 0; i < sizeof(SysCalls) / sizeof(struct SysCall); i++){
		if(strncmp(name, SysCalls[i].name, strlen(name)) == 0){
			addr = i;
		}
	}

	return addr;
}

bool IsNumber(char* op)
{
	if(strlen(op) >= 3 && op[0] == '0' && op[1] == 'x'){
		// check for hex
		for(unsigned int i = 2; i < strlen(op); i++){
			if(!('0' <= op[i] && op[i] <= '9') && !('a' <= op[i] && op[i] <= 'f') && !('A' <= op[i] && op[i] <= 'F')){
				return false;
			}
		}
	}else if(strlen(op) >= 3 && op[0] == '0' && op[1] == 'b'){
		// check for bit
		for(unsigned int i = 2; i < strlen(op); i++){
			if(op[i] != '0' && op[i] != '1'){
				return false;
			}
		}
	}else{
		for(unsigned int i = 0; i < strlen(op); i++){
			if(op[i] < '0' || op[i] > '9'){
				return false;
			}
		}
	}
	return true;
}

int CheckForHiLo(char* op)
{
	int id = -1;
	
	if(strlen(op) < 4){
		return 0;
	}

	for(int i = 0; i < sizeof(hilotable) / sizeof(char*); i++){
		if(strncmp(op, hilotable[i], strlen(hilotable[i])) == 0){
			id = i;
			break;
		}
	}
	if(id == -1){
		return 0;
	}
	if(*(op + strlen(hilotable[id])) != '('){
		return -1;
	}
	if(*(op + strlen(op) - 1) != ')'){
		return -1;
	}

	return 1;
}

int DecodeHiLo(char* op, unsigned int* pMask, int* pShift)
{
	int id = -1;
	char temp[8] = "";
	char* pCurr = op;

	for(int i = 0; i < sizeof(hilotable) / sizeof(char*); i++){
		if(strncmp(op, hilotable[i], strlen(hilotable[i])) == 0){
			id = i;
			break;
		}
	}
	if(id == -1){
		*pMask = 0xffffffff;
		*pShift = 0;
		return 0;
	}
	while(*pCurr != '(' && *pCurr != 0){
		pCurr = pCurr + 1;
	}
	*pMask = masktable[id];
	*pShift = shifttable[id];

	return pCurr - op;
}