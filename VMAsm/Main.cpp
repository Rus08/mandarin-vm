#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "VMAsm.h"
#include "Util.h"


char* pSource = 0;
char* pDataSource = 0;
struct Segment CodeSeg;
struct Segment DataSeg;


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
		strncpy(pSeg->pLabels[curr_label].name, pBegin, pCurr - pBegin);
		pSeg->pLabels[curr_label].string_num = i;
		memset(pBegin, ' ', pCurr - pBegin);
		curr_label = curr_label + 1;
	}
	pSeg->pLabels = (struct Label*)realloc(pSeg->pLabels, sizeof(struct Label) * curr_label);
	pSeg->LabelsNum = curr_label;

	return curr_label;
}


int main(int argc, char* argv[])
{
	FILE* fp = 0;
	int	file_size = 0;
	int code_size = 0;
	

	if(argc != 4){
		printf("Use VMAsm file.in code.out data.out.\n");
		return 0;
	}
	
	fp = fopen(argv[1], "rb");
	if(fp == 0){
		printf("File %s not found.\n", argv[1]);
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	pSource = (char*)malloc(file_size);
	fread(pSource, 1, file_size, fp);
	fclose(fp);

	// clear \r
	for(int i = 0; i < file_size; i++){
		if(pSource[i] == '\r'){
			pSource[i] = ' ';
		}
	}

	memset(&CodeSeg, 0, sizeof(struct Segment));
	memset(&DataSeg, 0, sizeof(struct Segment));
	CodeSeg.type = SEG_CODE;
	DataSeg.type = SEG_DATA;
	// clear all comments
	ClearComments(pSource, file_size);

	pDataSource = strstr(pSource, ".DATA");

	if(pDataSource == NULL){
		code_size = file_size;
	}else{
		code_size = pDataSource - pSource;
	}
	// parse code
	CodeSeg.StringsNum = MakeStringsMap(pSource, code_size, &CodeSeg);
	CodeSeg.LabelsNum = MakeLabelsMap(&CodeSeg);
	
	// parse data
	if(pDataSource != NULL){
		memset(pDataSource, ' ', strlen(".DATA"));
		DataSeg.StringsNum = MakeStringsMap(pDataSource, file_size - code_size, &DataSeg);
		DataSeg.LabelsNum = MakeLabelsMap(&DataSeg);
	}
	if(ProcessCode(pSource, code_size, &CodeSeg, &DataSeg) != 0){
		return -1;
	}
	if(ProcessData(pDataSource, file_size - code_size, &DataSeg) != 0){
		return -1;
	}
	

	free(pSource);

	// write code segment
	fp = fopen(argv[2], "wb");
	if(fp == 0){
		printf("Can't open file %s for writing.\n", argv[2]);
		return 0;
	}

	fwrite(CodeSeg.pBinary, 1, CodeSeg.pStrings[CodeSeg.StringsNum - 1].offset + CodeSeg.pStrings[CodeSeg.StringsNum - 1].binary_size, fp);
	fclose(fp);
	
	if(pDataSource != NULL){
		fp = fopen(argv[3], "wb");
		if(fp == 0){
			printf("Can't open file %s for writing.\n", argv[3]);
			return 0;
		}
		fwrite(DataSeg.pBinary, 1, DataSeg.pStrings[DataSeg.StringsNum - 1].offset + DataSeg.pStrings[DataSeg.StringsNum - 1].binary_size, fp);
		fclose(fp);

		free(DataSeg.pBinary);
		free(DataSeg.pLabels);
		free(DataSeg.pStrings);
	}

	free(CodeSeg.pBinary);
	free(CodeSeg.pLabels);
	free(CodeSeg.pStrings);

	return 0;
}