#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "VMAsm.h"
#include "Util.h"


int CodeDataInstruction(struct String* pString, int StringNum, struct Segment* pCodeSeg, struct Segment* pDataSeg, char* pOutBuf, int StringOffset);


int ProcessData(char* pSource, int code_size, struct Segment* pCodeSeg, struct Segment* pDataSeg, int StringOffset)
{

	if(pDataSeg->StringsNum > 0){
		pDataSeg->pBinary = (char*)malloc(pDataSeg->pStrings[pDataSeg->StringsNum - 1].offset + pDataSeg->pStrings[pDataSeg->StringsNum - 1].binary_size);
		for(int i = 0; i < pDataSeg->StringsNum; i++){
			CodeDataInstruction(pDataSeg->pStrings, i, pDataSeg, pCodeSeg, pDataSeg->pBinary + pDataSeg->pStrings[i].offset, StringOffset);
		}
	}

	return 0;
}

int CalcDataSizeAndOffset(struct Segment* pSeg, int StringOffset)
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
			printf("Error. Invalid instruction at: %d line.\n", StringOffset + i);
			return -1;
		}
		if(pSeg->pStrings[i].id < GetDataId("include")){
			if(pSeg->pStrings[i].repeat != 0){
				if(pSeg->pStrings[i].opnum > 1){
					printf("Error. Invalid operand count at line %d!\n", StringOffset + i);
					return -1;
				}
				pSeg->pStrings[i].binary_size = GetDataSize(pSeg->pStrings[i].id) * pSeg->pStrings[i].repeat;
			}else{
				pSeg->pStrings[i].binary_size = GetDataSize(pSeg->pStrings[i].id) * pSeg->pStrings[i].opnum;
			}
		}else{
			if(pSeg->pStrings[i].opnum != 1){
				printf("Error. Invalid operand count at %d line.\n", StringOffset + i);
				return -1;
			}
			pSeg->pStrings[i].binary_size = GetFileSize(pSeg->pStrings[i].op[0]);
			if(pSeg->pStrings[i].binary_size == -1){
				printf("Error. File %s in include at line %d not found!\n", pSeg->pStrings[i].op[0], StringOffset + i);
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

int CodeDataInstruction(struct String* pStrings, int StringNum, struct Segment* pCodeSeg, struct Segment* pDataSeg, char* pOutBuf, int StringOffset)
{
	struct String* pString = &pStrings[StringNum];
	int lastopintflag = -1;
	unsigned int op = 0;

	switch(pString->id){
		case VM_DWORD:
		{
			if(pString->repeat == 0){
				for(int i = 0; i < pString->opnum; i++){
					if(DecodeOperand(pString->op[i], 0xffffffff, &lastopintflag, StringOffset + StringNum, pCodeSeg, pDataSeg, &op) != 0){
						return -1;
					}
					((unsigned int*)pOutBuf)[i] = op;
				}
			}else{
				for(int i = 0; i < pString->repeat; i++){
					if(DecodeOperand(pString->op[0], 0xffffffff, &lastopintflag, StringOffset + StringNum, pCodeSeg, pDataSeg, &op) != 0){
						return -1;
					}
					((unsigned int*)pOutBuf)[i] = op;
				}
			}
		}
		break;
		case VM_WORD:
		{
			if(pString->repeat == 0){
				for(int i = 0; i < pString->opnum; i++){
					if(DecodeOperand(pString->op[i], 0xffff, &lastopintflag, StringOffset + StringNum, pCodeSeg, pDataSeg, &op) != 0){
						return -1;
				 	}
					((unsigned short*)pOutBuf)[i] = op;
				}
			}else{
				for(int i = 0; i < pString->repeat; i++){
					if(DecodeOperand(pString->op[0], 0xffff, &lastopintflag, StringOffset + StringNum, pCodeSeg, pDataSeg, &op) != 0){
						return -1;
					}
					((unsigned short*)pOutBuf)[i] = op;
				}
			}
		}
		break;
		case VM_BYTE:
		{
			if(pString->repeat == 0){
				for(int i = 0; i < pString->opnum; i++){
					if(DecodeOperand(pString->op[i], 0xff, &lastopintflag, StringOffset + StringNum, pCodeSeg, pDataSeg, &op) != 0){
						return -1;
					}
					((unsigned char*)pOutBuf)[i] = op;
				}
			}else{
				for(int i = 0; i < pString->repeat; i++){
					if(DecodeOperand(pString->op[0], 0xff, &lastopintflag, StringOffset + StringNum, pCodeSeg, pDataSeg, &op) != 0){
						return -1;
					}
					((unsigned char*)pOutBuf)[i] = op;
				}
			}
		}
		break;
		case VM_INCLUDE:
		{
			FILE* fp = 0;

			if(pString->opnum != 1){
				printf("Error. Instruction include at line %d can't have %d operands!\n", StringOffset + StringNum, pString->opnum);
				return -1;
			}

			fp = fopen(pString->op[0], "rb");
			/*if(fp == 0){
				printf("Error. File %s not found!\n", pString->op[0]);
				return -1;
			}*/
			fread(pOutBuf, 1, pString->binary_size, fp);
			fclose(fp);
		}
		break;
	};



	return 0;
}