#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "VMAsm.h"

int ParseString(struct String* pString)
{
	char* pBegin = pString->pBegin;
	char* pCurr = pString->pBegin;
	char* pEnd = pString->pEnd;
	int opnum = 0;

	// skip spacings
	while((*pCurr == ' ' || *pCurr == '\t') && pCurr != pEnd){
		pCurr = pCurr + 1;
	}

	if(pCurr == pEnd){
	// empty string
		pString->instr_name = 0;
		return 0;
	}

	pBegin = pCurr;

	// find next space
	while(*pCurr != ' ' && *pCurr != '\t' && *pCurr != '[' && pCurr != pEnd){
		pCurr = pCurr + 1;
	}
	pString->instr_name = pBegin;

	// check for repeat modifier
	if(*pCurr == '['){
		char temp[16] = { 0 };
		*pCurr = 0; // make name 0 ended string
		pBegin = pCurr + 1;

		while(*pCurr != ']' && pCurr != pEnd){
		// need checks for string end, and bad characters
			pCurr = pCurr + 1;
		}
		strncpy(temp, pBegin, pCurr - pBegin);
		pString->repeat = atoi(temp);
		if(pString->repeat <= 0){
			printf("Error. Invalid repeat modifier!\n");
			return -1;
		}

		if(pCurr == pEnd){
			// some error
			return -1;
		}
		pCurr = pCurr + 1;
	}else{
		*pCurr = 0;
		pCurr = pCurr + 1;
	}

	// skip spacings
	while(*pCurr == ' ' || *pCurr == '\t'){
		pCurr = pCurr + 1;
	}
	if(pCurr == pEnd){
	// no operand instruction
		return 0;
	}
	// count operand number
	pBegin = pCurr;
	opnum = opnum + 1;
	while(pCurr < pEnd){
		if(*pCurr == ','){
			opnum = opnum + 1;
		}
		pCurr = pCurr + 1;
	}
	pString->op = (char**)malloc(opnum * sizeof(char*));
	pString->opnum = opnum;

	pCurr = pBegin;
	// parse operands by order
	for(int i = 0; i < opnum; i++){
		pBegin = pCurr;
		while(*pCurr != ',' && pCurr != pEnd){
			pCurr = pCurr + 1;
		}
		pString->op[i] = pBegin;
		// skip spacing in reverse order
		char* pTemp = pCurr - 1;
		while(*pTemp == ' ' || *pTemp == '\t'){
			pTemp = pTemp - 1;
		}
		// make operand ended string
		*(pTemp + 1) = 0;
		if(pCurr < pEnd){
			pCurr = pCurr + 1;
			
			// skip spacings
			while((*pCurr == ' ' || *pCurr == '\t') && pCurr != pEnd){
				pCurr = pCurr + 1;
			}
		}
		assert(pCurr <= pEnd);
		if(pCurr == pEnd && i < (opnum - 1)){
			// end of string
			return -1;
		}
	}
	return 0;
}