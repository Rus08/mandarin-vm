#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "VMAsm.h"
#include "../../Execute32Bit.h"
#include "Util.h"


int CodeInstruction(struct String* pString, int StringNum, struct Segment* pCodeSeg, struct Segment* pDataSeg, char* pOutBuf);


int ProcessCode(char* pSource, int code_size, struct Segment* pCodeSeg, struct Segment* pDataSeg)
{
	pCodeSeg->pBinary = (char*)malloc(pCodeSeg->pStrings[pCodeSeg->StringsNum - 1].offset + pCodeSeg->pStrings[pCodeSeg->StringsNum - 1].binary_size);

	for(int i = 0; i < pCodeSeg->StringsNum; i++){
		int rc = 0;
		if(pCodeSeg->pStrings[i].instr_name == NULL){
			continue;
		}

		// labels in both segments
		int id = pCodeSeg->pStrings[i].id;
		rc = CodeInstruction(pCodeSeg->pStrings, i, IfCodeSeg(id) ? pCodeSeg : NULL, IfDataSeg(id) ? pDataSeg : NULL, pCodeSeg->pBinary + pCodeSeg->pStrings[i].offset);
		
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
	unsigned int op[3];

	if(pString->instr_name == NULL){
		return 0;
	}
	StringNum = StringNum + 1;
	if(pString->opnum != GetOperandCount(pString->id)){
		printf("Error. Instruction '%s' at line %d can't have %d operands!\n", pString->instr_name, StringNum, pString->opnum);
		return -1;
	}
	if(pString->id == VM_SYSCALL){
		op[0] = GetSysCallAddr(pString->op[0]);
		lastopintflag = 1;
		if(op[0] == -1){
			printf("Error. Invalid API function %s at line %d!\n", pString->op[0], StringNum);
			return -1;
		}
	}else{
		for(int i = 0; i < GetOperandCount(pString->id); i++){
			if(DecodeOperand(pString->op[i], GetIntMaxSize(pString->id), &lastopintflag, IfSigned(pString->id), StringNum, pString->offset, pCodeSeg, pDataSeg, &op[i]) != 0){
				return -1;
			}
			if(i < (GetOperandCount(pString->id) - 1) && lastopintflag == 1){
				// not last operand isn't register << wtf this comment?
				printf("Error. Operand %d at line %d can't be integer!\n", i /*+ 1*/, StringNum);
				return -1;
			}
		}
	}

	if(IfInteger(pString->id) == false && lastopintflag == 1){
		printf("Error. Instruction %s at line %d can't have integer operands!\n", pString->instr_name, StringNum);
		return -1;
	}
	if(IfInteger(pString->id) == true && lastopintflag == 0){
		printf("Error. Instruction %s at line %d should have integer last operand!\n", pString->instr_name, StringNum);
		return -1;
	}
	if(IfVector(pString->id) == false && pString->repeat > 0){
		printf("Error. Instruction %s at line %d can't have repeat modifier!\n", pString->instr_name, StringNum);
		return -1;
	}

	if(pString->repeat != 0){
		pString->repeat = pString->repeat - 1;
	}
	
	switch(GetOperandCount(pString->id))
	{
		case 3:
		{ 
			// 3 operand instructions
			if(IfVector(pString->id) == false){
				if(IfInteger(pString->id) == false){
					instr = op[2] << 14;
				}else{
					instr = op[2] << 5;
				}
				instr = (instr | op[1]) << 5;
				instr = (instr | op[0]) << 8;
			}else{
				instr = op[2] << 9;
				instr = (instr | op[1]) << 5;
				instr = (instr | op[0]) << 5;
				instr = (instr | pString->repeat) << 8;
			}
		}
		break;
		case 2:
		{
			if(IfVector(pString->id) == false){
				if(IfInteger(pString->id) == false){
					instr = op[1] << 19;
				}else{
					instr = op[1] << 5;
				}
				instr = (instr | op[0]) << 8;
			}else{
				if(IfInteger(pString->id) == false){
					instr = op[1] << 14;
				}else{
					// memory integer instructions
					instr = op[1] << 5;
				}
				instr = (instr | op[0]) << 5;
				instr = (instr | pString->repeat) << 8;
			}
		}
		break;
		case 1:
		case 0:
		{
			if(IfInteger(pString->id) == false){
				instr = op[0] << 27;
			}else{
				instr = op[0] << 8;
			}
		}
		break;
	}

	instr = instr | pString->id;
	*(unsigned int*)pOutBuf = instr;

	return 0;
}
