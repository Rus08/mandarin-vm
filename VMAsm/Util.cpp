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
	for(unsigned int i = 0; i < strlen(op); i++){
		if(op[i] < '0' || op[i] > '9'){
			return false;
		}
	}
	return true;
}