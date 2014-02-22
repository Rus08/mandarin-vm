#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "VMAsm.h"
#include "Util.h"


int CalcSizeAndOffset(struct Segment* pSeg);
int CodeInstruction(struct String* pString, int StringNum, struct Segment* pSeg, char* pOutBuf);




int ProcessCode(char* pSource, int code_size, struct Segment* pCodeSeg, struct Segment* pDataSeg)
{
	for(int i = 0; i < pCodeSeg->StringsNum; i++){
		ParseString(&pCodeSeg->pStrings[i]);
	}

	CalcSizeAndOffset(pCodeSeg);

	pCodeSeg->pBinary = (char*)malloc(pCodeSeg->pStrings[pCodeSeg->StringsNum - 1].offset + pCodeSeg->pStrings[pCodeSeg->StringsNum - 1].binary_size);

	for(int i = 0; i < pCodeSeg->StringsNum; i++){
		int rc = 0;

		if(pCodeSeg->pStrings[i].id < GetId("load")){
			// labels at code segment
			rc = CodeInstruction(pCodeSeg->pStrings, i, pCodeSeg, pCodeSeg->pBinary + pCodeSeg->pStrings[i].offset);
		}else{
			// labels at data segment
			rc = CodeInstruction(pCodeSeg->pStrings, i, pDataSeg, pCodeSeg->pBinary + pCodeSeg->pStrings[i].offset);
		}

		if(rc != 0){
			return -1;
		}
	}

	return 0;
}

int MakeLabelsMap(struct Segment* pSeg)
{
	int labels_num = 0;
	int curr_label = 0;

	for(int i = 0; i < pSeg->StringsNum; i++){
		for(int b = 0; b < (pSeg->pStrings[i].pEnd - pSeg->pStrings[i].pBegin); b++){
			if(pSeg->pStrings[i].pBegin[b] == ':'){
				labels_num = labels_num + 1;
			}
		}
	}

	pSeg->pLabels = (struct Label*)malloc(sizeof(struct Label) * labels_num);
	memset(pSeg->pLabels, 0, sizeof(struct Label) * labels_num);

	for(int i = 0; i < pSeg->StringsNum; i++){
		char* pCurr = pSeg->pStrings[i].pBegin;
		for(int b = 0; b < pSeg->pStrings[i].pEnd - pSeg->pStrings[i].pBegin; b++){
			if(*pCurr == ':'){
				char* pBegin = pSeg->pStrings[i].pBegin;
				pSeg->pLabels[curr_label].string_num = i;
				// skip any spacings
				while(*pBegin == ' ' || *pBegin == '\t'){
					pBegin = pBegin + 1;
				}
				if(pCurr - pBegin <= 1){
					return -1; // error
				}
				strncpy(pSeg->pLabels[curr_label].name, pBegin, pCurr - pBegin);
				memset(pBegin, ' ', pCurr - pBegin + 1);
				curr_label = curr_label + 1;
				break;
			}
			pCurr = pCurr + 1;
		}
	}

	return curr_label;
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
		if(pSeg->pStrings[i].id == -1){
			// Invalid instruction
			printf("Invalid instruction at line: %d.\n", i);
			return -1;
		}
		pSeg->pStrings[i].binary_size = GetSize(pSeg->pStrings[i].id);
	}
	// calc offset
	for(int i = 0; i < pSeg->StringsNum; i++){
		pSeg->pStrings[i].offset = offset;
		offset = offset + pSeg->pStrings[i].binary_size;
	}
	return 0;
}

int CodeInstruction(struct String* pStrings, int StringNum, struct Segment* pSeg, char* pOutBuf)
{
	struct String* pString = &pStrings[StringNum];
	unsigned int instr = 0;
	int lastopintflag = 0;
	bool syscallflag = false;
	unsigned int op[3];

	if(pString->instr_name == NULL){
		return 0;
	}
	if(pString->opnum != GetOperandCount(pString->id)){
		printf("Error. Instruction '%s' at line %d can't have %d operands!\n", pString->instr_name, StringNum, pString->opnum);
		return -1;
	}
	if(pString->id == GetId("syscall")){
		op[0] = GetSysCallAddr(pString->op[0]);
		if(op[0] == -1){
			printf("Error. Invalid API function %s at line %d!\n", pString->op[0], StringNum);
			return -1;
		}
	}else{
		for(int i = 0; i < GetOperandCount(pString->id); i++){
			/*if(pString->op[i] == NULL){
				// where is operand?
				printf("Error. Instruction '%s' at line %d can't have %d operands!\n", pString->instr_name, StringNum, i);
				return -1;
			}*/
			if(DecodeOperand(pString->op[i], GetIntMaxSize(pString->id), &lastopintflag, StringNum, pSeg, &op[i]) != 0){
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
	instr = (instr | pString->id) << 1;
	instr = (instr | 1); // 32 bit flag
	*(int*)pOutBuf = instr;

	return 0;
}