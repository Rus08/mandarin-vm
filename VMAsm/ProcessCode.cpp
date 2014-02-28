#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "VMAsm.h"
#include "Util.h"


int CalcSizeAndOffset(struct Segment* pSeg);
int CodeInstruction(struct String* pString, int StringNum, struct Segment* pCodeSeg, struct Segment* pDataSeg, char* pOutBuf);
int Code16BitInstruction(struct String* pStrings, int StringNum, char* pOutBuf);



int ProcessCode(char* pSource, int code_size, struct Segment* pCodeSeg, struct Segment* pDataSeg)
{
	for(int i = 0; i < pCodeSeg->StringsNum; i++){
		ParseString(&pCodeSeg->pStrings[i]);
	}

	CalcSizeAndOffset(pCodeSeg);

	pCodeSeg->pBinary = (char*)malloc(pCodeSeg->pStrings[pCodeSeg->StringsNum - 1].offset + pCodeSeg->pStrings[pCodeSeg->StringsNum - 1].binary_size);

	for(int i = 0; i < pCodeSeg->StringsNum; i++){
		int rc = 0;
		if(pCodeSeg->pStrings[i].instr_name == NULL){
			continue;
		}

		if(Get16BitId(pCodeSeg->pStrings[i].instr_name) == -1){
			if(pCodeSeg->pStrings[i].id < GetId("call")){
				// labels in both segments
				rc = CodeInstruction(pCodeSeg->pStrings, i, pCodeSeg, pDataSeg, pCodeSeg->pBinary + pCodeSeg->pStrings[i].offset);
			}else if(pCodeSeg->pStrings[i].id < GetId("load")){
				// labels at code segment
				rc = CodeInstruction(pCodeSeg->pStrings, i, pCodeSeg, NULL, pCodeSeg->pBinary + pCodeSeg->pStrings[i].offset);
			}else{
				// labels at data segment
				rc = CodeInstruction(pCodeSeg->pStrings, i, pDataSeg, NULL, pCodeSeg->pBinary + pCodeSeg->pStrings[i].offset);
			}
		}else{
			rc = Code16BitInstruction(pCodeSeg->pStrings, i, pCodeSeg->pBinary + pCodeSeg->pStrings[i].offset);
		}
		if(rc != 0){
			return -1;
		}
	}

	return 0;
}

int CalcSizeAndOffset(struct Segment* pSeg)
{
	// calc size of instructions and offset
	// need check for 16 bit instruction bad order
	int offset = 0;

	for(int i = 0; i < pSeg->StringsNum; i++){
		if(pSeg->pStrings[i].instr_name == NULL){
			pSeg->pStrings[i].binary_size = 0;
			continue;
		}
		pSeg->pStrings[i].id = GetId(pSeg->pStrings[i].instr_name);
		pSeg->pStrings[i].binary_size = 4/*GetSize(pSeg->pStrings[i].id)*/;

		if(pSeg->pStrings[i].id == -1){
			// check for 16 bit instruction
			pSeg->pStrings[i].id = Get16BitId(pSeg->pStrings[i].instr_name);
			pSeg->pStrings[i].binary_size = 2;
		}
		if(pSeg->pStrings[i].id == -1){
			// Invalid instruction
			printf("Invalid instruction at line: %d.\n", i);
			return -1;
		}
	}
	// calc offset
	for(int i = 0; i < pSeg->StringsNum; i++){
		pSeg->pStrings[i].offset = offset;
		offset = offset + pSeg->pStrings[i].binary_size;
	}
	return 0;
}

int CodeInstruction(struct String* pStrings, int StringNum, struct Segment* pCodeSeg, struct Segment* pDataSeg, char* pOutBuf)
{
	struct String* pString = &pStrings[StringNum];
	unsigned int instr = 0;
	int lastopintflag = 0;
	bool syscallflag = false;
	unsigned int op[3];
	int lflag[2];

	if(pString->instr_name == NULL){
		return 0;
	}
	StringNum = StringNum + 1;
	if(pString->opnum != GetOperandCount(pString->id)){
		printf("Error. Instruction '%s' at line %d can't have %d operands!\n", pString->instr_name, StringNum, pString->opnum);
		return -1;
	}
	if(pString->id == GetId("syscall")){
		op[0] = GetSysCallAddr(pString->op[0]);
		lastopintflag = 1;
		if(op[0] == -1){
			printf("Error. Invalid API function %s at line %d!\n", pString->op[0], StringNum);
			return -1;
		}
	}else{
		if(pString->id == GetId("memcpy")){
			for(int b = 0; b < 2; b++){
				if(pString->op[b][0] == 'l'){
					// local memory
					lflag[b] = 1;
					pString->op[b] = pString->op[b] + 2;
				}else if(pString->op[b][0] == 'g'){
					// global memory
					lflag[b] = 0;
					pString->op[b] = pString->op[b] + 2;
				}else{
					// no modifier - global memory
					lflag[b] = 0;
				}
			}
		}

		for(int i = 0; i < GetOperandCount(pString->id); i++){
			/*if(pString->op[i] == NULL){
				// where is operand?
				printf("Error. Instruction '%s' at line %d can't have %d operands!\n", pString->instr_name, StringNum, i);
				return -1;
			}*/
			if(DecodeOperand(pString->op[i], GetIntMaxSize(pString->id), &lastopintflag, StringNum, pCodeSeg, pDataSeg, &op[i]) != 0){
				return -1;
			}
			if(i < (GetOperandCount(pString->id) - 1) && lastopintflag == 1){
				// not last operand isn't register
				printf("Error. Operand %d at line %d can't be integer!\n", i /*+ 1*/, StringNum);
				return -1;
			}
		}
	}

	/*if(GetOperandCount(pString->id) == 0 && (pString->op[0] != NULL || pString->op[1] != NULL || pString->op[2] != NULL)){
		printf("Error. Instruction '%s' at line %d can't have operands!\n", pString->instr_name, StringNum);
		return -1;
	}*/
	if(pString->repeat != 0){
		pString->repeat = pString->repeat - 1;
	}

	if(pString->id <= GetId("xor")){
		// 3 operand instructions
		instr = op[2] << 5;
		instr = (instr | op[1]) << 1;
		instr = (instr | lastopintflag) << 5;
		instr = (instr | op[0]) << 5;
		instr = (instr | pString->repeat) << 6;
	}
	if(GetId("xor") < pString->id && pString->id <= GetId("not")){
		// 2 operand instructions
		instr = op[1] << 1;
		instr = (instr | lastopintflag) << 5;
		instr = (instr | op[0]) << 5;
		instr = (instr | pString->repeat) << 6;
	}
	if(GetId("not") < pString->id && pString->id <= GetId("syscall")){
		// 1 operand instructions
		instr = op[0] << 1;
		instr = (instr | lastopintflag) << 6;
	}
	if(GetId("jmp") < pString->id && pString->id <= GetId("ret")){
		// 0 operand instructions
	}
	if(GetId("ret") < pString->id && pString->id <= GetId("storelb")){
		// 2 operand memory instructions
		instr = op[1] << 1;
		instr = (instr | lastopintflag) << 5;
		instr = (instr | op[0]) << 5;
		instr = (instr | pString->repeat) << 6;
	}
	if(GetId("storelb") < pString->id && pString->id <= GetId("memcpy")){
		// 3 operand memory instructions
		instr = op[2] << 5;
		instr = (instr | op[1]) << 1;
		instr = (instr | lastopintflag) << 5;
		instr = (instr | op[0]) << 5;
		int flags = (lflag[1] << 1) | lflag[0];
		instr = (instr | flags) << 6;
	}
	instr = (instr | pString->id) << 1;
	instr = (instr | 1); // 32 bit flag
	*(unsigned int*)pOutBuf = instr;

	return 0;
}


int Code16BitInstruction(struct String* pStrings, int StringNum, char* pOutBuf)
{
	struct String* pString = &pStrings[StringNum];
	unsigned short instr = 0;
	int lastopintflag = 0;
	unsigned int op[3];

	if(pString->instr_name == NULL){
		return 0;
	}
	if(pString->opnum != GetOperandCount(pString->id)){
		printf("Error. Instruction '%s' at line %d can't have %d operands!\n", pString->instr_name, StringNum, pString->opnum);
		return -1;
	}

	for(int i = 0; i < GetOperandCount(pString->id); i++){
		if(DecodeOperand(pString->op[i], GetIntMaxSize(pString->id), &lastopintflag, StringNum, NULL, NULL, &op[i]) != 0){
			return -1;
		}
		if(lastopintflag == 1){
			// operand isn't register
			printf("Error. Operand %d at line %d can't be integer!\n", i /*+ 1*/, StringNum);
			return -1;
		}
	}
	instr = (instr | op[2]) << 4;
	instr = (instr | op[1]) << 4;
	instr = (instr | op[0]) << 3;
	instr = (instr | pString->id) << 1;
	instr = (instr | 0);
	*(unsigned short*)pOutBuf = instr;

	return 0;
}