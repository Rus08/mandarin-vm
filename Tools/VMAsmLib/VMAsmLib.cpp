#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "VMAsm.h"
#include "Util.h"

int ClearComments(char* pAsm, int file_size)
{
	for(int i = 0; i < file_size - 1; i++){
		if(pAsm[i] == '/' && pAsm[i + 1] == '/'){
			char* pTemp = pAsm + i;
			while(*pTemp != '\n'){
				*pTemp = ' ';
				pTemp = pTemp + 1;
			}
		}
	}

	return 0;
}

int MakeStringsMap(char* pAsm, int file_size, struct Segment* pSeg)
{
	char* pBegin = 0;
	int strings_num = 0;
	int curr_str = 0;

	for(int i = 0; i < file_size; i++){
		if(pAsm[i] == '\n'){
			strings_num = strings_num + 1;
		}
	}
	strings_num = strings_num + 1;
	

	pSeg->pStrings = (struct String*)malloc(sizeof(struct String) * strings_num);
	memset(pSeg->pStrings, 0, sizeof(struct String) * strings_num);

	pBegin = pAsm;
	for(int i = 0; i < file_size; i++){	
		if(pAsm[i] == '\n'){
			pSeg->pStrings[curr_str].pBegin = pBegin;
			pSeg->pStrings[curr_str].pEnd = pAsm + i;
			pBegin = pAsm + i + 1; // need check
			curr_str = curr_str + 1;
		}
	}
	// handle last string
	pSeg->pStrings[strings_num - 1].pBegin = pBegin;
	pSeg->pStrings[strings_num - 1].pEnd = pAsm + file_size;

	pSeg->StringsNum = strings_num;
	return strings_num;
}

int MakeLabelsMap(struct Segment* pSeg)
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
		if(pSeg->type == SEG_CODE){
			if(*(pCurr - 1) == ':'){
				if((pCurr - pBegin) < 2){
					return -1;
				}
			}else{
				continue;
			}
		}else{
			memset(temp, 0, sizeof(temp));
			strncpy(temp, pBegin, pCurr - pBegin);
			if(IsDataInstr(temp) == true){
				// no label
				continue;
			}
		}
		strncpy(pSeg->pLabels[curr_label].name, pBegin, pCurr - pBegin - (pSeg->type == SEG_CODE ? 1 : 0));
		pSeg->pLabels[curr_label].string_num = i;
		memset(pBegin, ' ', pCurr - pBegin);
		curr_label = curr_label + 1;
	}
	pSeg->pLabels = (struct Label*)realloc(pSeg->pLabels, sizeof(struct Label) * curr_label);
	pSeg->LabelsNum = curr_label;

	return curr_label;
}

int OnErrorExit(char* pSource, struct Segment* pCodeSeg, struct Segment* pDataSeg)
{
	if(pSource != NULL){
		free(pSource);
	}
	if(pCodeSeg->pStrings != NULL){
		free(pCodeSeg->pStrings);
	}
	if(pCodeSeg->pLabels != NULL){
		free(pCodeSeg->pLabels);
	}
	if(pCodeSeg->pBinary != NULL){
		free(pCodeSeg->pBinary);
	}
	free(pCodeSeg);

	if(pDataSeg->pStrings != NULL){
		free(pDataSeg->pStrings);
	}
	if(pDataSeg->pLabels != NULL){
		free(pDataSeg->pLabels);
	}
	if(pDataSeg->pBinary != NULL){
		free(pDataSeg->pBinary);
	}
	free(pDataSeg);

	return 0;
}

int AsmFileToMemory(char* FileName, char** ppCode, int* pCodeSize, char** ppData, int* pDataSize)
{
	FILE* fp = 0;
	int	file_size = 0;
	int code_size = 0;
	char* pSource = 0;
	char* pDataSource = 0;
	struct Segment* pCodeSeg;
	struct Segment* pDataSeg;

	fp = fopen(FileName, "rb");
	if(fp == 0){
		printf("File %s not found.\n", FileName);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	pSource = (char*)malloc(file_size + 1); // +1 needed for case when file ended not with \n to place 0 terminator
	fread(pSource, 1, file_size, fp);
	fclose(fp);
	pSource[file_size] = 0;

	// clear \r
	for(int i = 0; i < file_size; i++){
		if(pSource[i] == '\r'){
			pSource[i] = ' ';
		}
	}
	
	pCodeSeg = (struct Segment*)malloc(sizeof(struct Segment));
	pDataSeg = (struct Segment*)malloc(sizeof(struct Segment));
	memset(pCodeSeg, 0, sizeof(struct Segment));
	memset(pDataSeg, 0, sizeof(struct Segment));
	pCodeSeg->type = SEG_CODE;
	pDataSeg->type = SEG_DATA;
	// clear all comments
	ClearComments(pSource, file_size);

	pDataSource = strstr(pSource, ".DATA");

	if(pDataSource == NULL){
		code_size = file_size;
	}else{
		code_size = pDataSource - pSource;
	}
	// parse code
	pCodeSeg->StringsNum = MakeStringsMap(pSource, code_size, pCodeSeg);
	pCodeSeg->LabelsNum = MakeLabelsMap(pCodeSeg);
	for(int i = 0; i < pCodeSeg->StringsNum; i++){
		ParseString(&pCodeSeg->pStrings[i]);
	}
	CalcSizeAndOffset(pCodeSeg);
	// parse data
	if(pDataSource != NULL){
		memset(pDataSource, ' ', strlen(".DATA"));
		pDataSeg->StringsNum = MakeStringsMap(pDataSource, file_size - code_size, pDataSeg);
		pDataSeg->LabelsNum = MakeLabelsMap(pDataSeg);
		for(int i = 0; i < pDataSeg->StringsNum; i++){
			ParseString(&pDataSeg->pStrings[i]);
		}
		CalcDataSizeAndOffset(pDataSeg, pCodeSeg->StringsNum);
	}
	if(ProcessCode(pSource, code_size, pCodeSeg, pDataSeg) != 0){
		OnErrorExit(pSource, pCodeSeg, pDataSeg);
		return -1;
	}
	if(ProcessData(pDataSource, file_size - code_size, pCodeSeg, pDataSeg, pCodeSeg->StringsNum) != 0){
		OnErrorExit(pSource, pCodeSeg, pDataSeg);
		return -1;
	}
	
	*ppCode = pCodeSeg->pBinary;
	*pCodeSize = pCodeSeg->pStrings[pCodeSeg->StringsNum - 1].offset + pCodeSeg->pStrings[pCodeSeg->StringsNum - 1].binary_size;
	if(pDataSource != NULL){
		*ppData = pDataSeg->pBinary;
		*pDataSize = pDataSeg->pStrings[pDataSeg->StringsNum - 1].offset + pDataSeg->pStrings[pDataSeg->StringsNum - 1].binary_size;
		free(pDataSeg->pLabels);
		free(pDataSeg->pStrings);
	}else{
		*ppData = NULL;
		*pDataSize = 0;
	}
	free(pCodeSeg->pLabels);
	free(pCodeSeg->pStrings);
	free(pCodeSeg);
	free(pDataSeg);
	free(pSource);

	return 0;
}