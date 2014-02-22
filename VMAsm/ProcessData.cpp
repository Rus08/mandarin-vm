#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "VMAsm.h"
#include "Util.h"


int CalcDataSizeAndOffset(struct Segment* pSeg);
int CodeDataInstruction(struct String* pString, int StringNum, struct Segment* pSeg, char* pOutBuf);


int ProcessData(char* pSource, int code_size, struct Segment* pDataSeg)
{

	for(int i = 0; i < pDataSeg->StringsNum; i++){
		ParseString(&pDataSeg->pStrings[i]);
	}

	CalcDataSizeAndOffset(pDataSeg);

	pDataSeg->pBinary = (char*)malloc(pDataSeg->pStrings[pDataSeg->StringsNum - 1].offset + pDataSeg->pStrings[pDataSeg->StringsNum - 1].binary_size);

	for(int i = 0; i < pDataSeg->StringsNum; i++){
		CodeDataInstruction(pDataSeg->pStrings, i, pDataSeg, pDataSeg->pBinary + pDataSeg->pStrings[i].offset);
	}

	return 0;
}

int MakeDataLabelsMap(struct Segment* pSeg)
{
	int curr_label = 0;
	char* pBegin = 0;
	char* pCurr = 0;
	char temp[64] = "";

	pSeg->pLabels = (struct Label*)malloc(sizeof(struct Label) * pSeg->StringsNum);
	memset(pSeg->pLabels, 0, sizeof(struct Label) * pSeg->StringsNum);

	for(int i = 0; i < pSeg->StringsNum; i++){
		pBegin = pSeg->pStrings[i].pBegin;
		// skip spacings
		while(*pBegin == ' ' || *pBegin == '\t'){
			pBegin = pBegin + 1;
		}
		if(pBegin == pSeg->pStrings[i].pEnd){
			continue;
		}
		pCurr = pBegin;
		// find next space
		while(*pCurr != ' ' && *pCurr != '\t' && pCurr != pSeg->pStrings[i].pEnd){
			pCurr = pCurr + 1;
		}
		memset(temp, 0, sizeof(temp));
		strncpy(temp, pBegin, pCurr - pBegin);
		if(IsDataInstr(temp) == true){
			// no label
			continue;
		}
		strncpy(pSeg->pLabels[curr_label].name, pBegin, pCurr - pBegin);
		pSeg->pLabels[curr_label].string_num = i;
		memset(pBegin, ' ', pCurr - pBegin);
		curr_label = curr_label + 1;
	}
	pSeg->LabelsNum = curr_label;

	return curr_label;
}


int CalcDataSizeAndOffset(struct Segment* pSeg)
{
	// calc size of instructions and offset
	// need check for 16 bit instruction bad order
	int offset = 0;
	
	for(int i = 0; i < pSeg->StringsNum; i++){
		if(pSeg->pStrings[i].instr_name == NULL){
			pSeg->pStrings[i].binary_size = 0;
			continue;
		}
		pSeg->pStrings[i].id = GetDataId(pSeg->pStrings[i].instr_name);
		if(pSeg->pStrings[i].id == -1){
			printf("Invalid instruction at: %d line.\n", i);
			return -1;
		}
		if(pSeg->pStrings[i].id < GetDataId("include")){
			pSeg->pStrings[i].binary_size = GetDataSize(pSeg->pStrings[i].id) * pSeg->pStrings[i].opnum;
		}else{
			if(pSeg->pStrings[i].opnum != 1){
				printf("Invalid operand count at %d line.\n", i);
				return -1;
			}
			pSeg->pStrings[i].binary_size = GetFileSize(pSeg->pStrings[i].op[0]);
			if(pSeg->pStrings[i].binary_size == -1){
				printf("Error. File %s not found!\n", pSeg->pStrings[i].op[0]);
				return -1;
			}
		}
	}

	// calc offset
	for(int i = 0; i < pSeg->StringsNum; i++){
		pSeg->pStrings[i].offset = offset;
		offset = offset + pSeg->pStrings[i].binary_size;
	}
	return 0;
}

int CodeDataInstruction(struct String* pStrings, int StringNum, struct Segment* pSeg, char* pOutBuf)
{
	struct String* pString = &pStrings[StringNum];
	int lastopintflag = -1;
	unsigned int op = 0;

	switch(pString->id){
		case VM_DWORD:
		{
			for(int i = 0; i < pString->opnum; i++){
				if(DecodeOperand(pString->op[i], 0xffffffff, &lastopintflag, StringNum, pSeg, &op) != 0){
					return -1;
				}
				((unsigned int*)pOutBuf)[i] = op;
			}
		}
		break;
		case VM_WORD:
		{
			for(int i = 0; i < pString->opnum; i++){
				if(DecodeOperand(pString->op[i], 0xffffffff, &lastopintflag, StringNum, pSeg, &op) != 0){
					return -1;
				}
				((unsigned short*)pOutBuf)[i] = op;
			}
		}
		break;
		case VM_BYTE:
		{
			for(int i = 0; i < pString->opnum; i++){
				if(DecodeOperand(pString->op[i], 0xffffffff, &lastopintflag, StringNum, pSeg, &op) != 0){
					return -1;
				}
				((unsigned char*)pOutBuf)[i] = op;
			}
		}
		break;
		case VM_INCLUDE:
		{
			FILE* fp = 0;

			if(pString->opnum != 1){
				printf("Error. Instruction include at line %d can't have %d operands!\n", StringNum, pString->opnum);
				return -1;
			}

			fp = fopen(pString->op[0], "rb");
			if(fp == 0){
				printf("Error. File %s not found!\n", pString->op[0]);
				return -1;
			}
			fread(pOutBuf, 1, pString->binary_size, fp);
			fclose(fp);
		}
		break;
	};



	return 0;
}