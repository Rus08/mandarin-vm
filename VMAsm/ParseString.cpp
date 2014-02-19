#include <stdlib.h>
#include <string.h>
#include "VMAsm.h"

int ParseString(struct String* pString)
{
	char* pBegin = pString->pBegin;
	char* pCurr = pString->pBegin;
	char* pEnd = pString->pEnd;

	// skip spacings
	while(*pCurr == ' ' || *pCurr == '\t'){
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
		char temp[3] = { 0 };
		*pCurr = 0; // make name 0 ended string
		pBegin = pCurr + 1;

		while(*pCurr != ']' && pCurr != pEnd){
		// need checks for string end, and bad characters
			pCurr = pCurr + 1;
		}
		strncpy(temp, pBegin, pCurr - pBegin);
		pString->repeat = atoi(temp);

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
	
	// parse operands by order
	for(int i = 0; i < 3; i++){
		pBegin = pCurr;
		while(*pCurr != ' ' && *pCurr != '\t' && *pCurr != ',' && pCurr != pEnd){
			pCurr = pCurr + 1;
		}
		pString->op[i] = pBegin;
		//pString->opsize[i] = pCurr - pBegin;

		// make operand 0 ended string
		*pCurr = 0;
		pCurr = pCurr + 1;
		
		// skip spacings
		while(*pCurr == ' ' || *pCurr == '\t' || *pCurr == ','){
			pCurr = pCurr + 1;
		}
		if(pCurr == pEnd){
			// end of string
			break;
		}
	}
	return 0;
}