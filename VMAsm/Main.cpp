#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "VMAsm.h"
#include "Util.h"


char* pAsm = 0;
struct String* pStrings = 0;
struct Label* pLabels = 0;

int StringsNum = 0;
int LabelsNum = 0;


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

int MakeStringsMap(char* pAsm, int file_size)
{
	int strings_num = 0;
	int curr_str = 0;

	for(int i = 0; i < file_size; i++){
		if(pAsm[i] == '\n'){
			strings_num = strings_num + 1;
		}
	}

	pStrings = (struct String*)malloc(sizeof(struct String) * (strings_num + 1));
	memset(pStrings, 0, sizeof(struct String) * (strings_num + 1));

	pStrings[0].pBegin = pAsm;
	for(int i = 0; i < file_size; i++){
		if(pAsm[i] == '\n'){
			pStrings[curr_str].pEnd = pAsm + i;
			pStrings[curr_str + 1].pBegin = pAsm + i + 1; // need check
			curr_str = curr_str + 1;
		}
	}

	return strings_num;
}

int MakeLabelsMap(char* pAsm, int file_size)
{
	int labels_num = 0;
	int curr_label = 0;

	for(int i = 0; i < file_size; i++){
		if(pAsm[i] == ':'){
			labels_num = labels_num + 1;
		}
	}

	pLabels = (struct Label*)malloc(sizeof(struct Label) * labels_num);
	memset(pLabels, 0, sizeof(struct Label) * labels_num);

	for(int i = 0; i < StringsNum; i++){
		char* pTemp = pStrings[i].pBegin;
		for(int b = 0; b < pStrings[i].pEnd - pStrings[i].pBegin; b++){
			if(*pTemp == ':'){
				char* pBegin = pStrings[i].pBegin;
				pLabels[curr_label].string_num = i;
				// skip any spacings
				while(*pBegin == ' ' || *pBegin == '\t'){
					pBegin = pBegin + 1;
				}
				if(pTemp - pBegin <= 1){
					return -1; // error
				}
				strncpy(pLabels[curr_label].name, pBegin, pTemp - pBegin);
				memset(pBegin, ' ', pTemp - pBegin + 1);
				curr_label = curr_label + 1;
				break;
			}
			pTemp = pTemp + 1;
		}
	}

	return labels_num;
}

int CalcSizeAndOffset(struct String* pStrings)
{
	// calc size of instructions and offset
	// need check for 16 bit instruction bad order
	int offset = 0;
	for(int i = 0; i < StringsNum; i++){
		int id = -1;
		if(pStrings[i].instr_name == NULL){
			pStrings[i].offset = offset;
			continue;
		}
		id = GetId(pStrings[i].instr_name);
		if(id == -1){
			// Invalid instruction
			printf("Invalid instruction at: %d line.\n", i);
			return -1;
		}
		pStrings[i].id = id;
		pStrings[i].instruction_size = GetSize(id);
		pStrings[i].offset = offset;
		offset = offset + GetSize(id);
	}

	return 0;
}

int DecodeOperand(char* Op, int IntMaxSize, int* pLastopintflag, bool SysCall, int StringNum)
{
	int op = 0;

	if(IsNumber(Op) == true){
		// operand is number
		op = atoi(Op);
		/*if(*pLastopintflag != -1){
			printf("Error.Invalid operand at line %d!\n", StringNum);
			return -1;
		}*/
		*pLastopintflag = 1;
		if(op > IntMaxSize){
			printf("Error. Operand at line %d is too big!\n", StringNum);
			return -1;
		}
	}else{
		if(Op[0] != 'r'){
			// operand is address label
			if(SysCall == false){
				op = GetLabelAddr(Op);
				if(op == -1){
					printf("Error. Label %s at line %d not found!\n", Op, StringNum);
					return -1;
				}
			}else{
				op = GetSysCallAddr(Op);
				if(op == -1){
					printf("Error. API function %s at line %d not found!\n", Op, StringNum);
					return -1;
				}
			}
			if(*pLastopintflag != -1){
				printf("Error.Invalid operand at line %d!\n", StringNum);
				return -1;
			}
			*pLastopintflag = 1;
		}else{
			// operand is register
			if(IsNumber(Op + 1) != true){
				// register number isn't number
				printf("Error. Invalid operand at line %d!\n", StringNum);
				return -1;
			}
			op = atoi(Op + 1);
			if(op > 31){
				// register number is too high
				printf("Error. Register number is too high at line %d!\n", StringNum);
				return -1;
			}
			*pLastopintflag = 0;
		}
	}

	return op;
}


int CodeInstruction(struct String* pStrings, int StringNum, struct Label* pLabels, char* pOutBuf)
{
	struct String* pString = &pStrings[StringNum];
	unsigned int instr = 0;
	int lastopintflag = -1;
	bool syscallflag = false;
	int op[3];

	if(pString->instr_name == NULL){
		return 0;
	}
	if(pString->id == GetId("syscall")){
		syscallflag = true;
	}

	for(int i = 0; i < GetOperandCount(pString->id); i++){
		if(pString->op[i] == NULL){
			// where is operand?
			printf("Error. Instruction '%s' at line %d can't have %d operands!\n", pString->instr_name, StringNum, i);
			return -1;
		}
		op[i] = DecodeOperand(pString->op[i], GetIntMaxSize(pString->id), &lastopintflag, syscallflag, StringNum);
		
		if(i < (GetOperandCount(pString->id) - 1) && lastopintflag == 1){
			// not last operand isn't register
			printf("Error. Operand %d at line %d can't be integer!\n", i /*+ 1*/, StringNum);
			return -1;
		}
	}

	if(GetOperandCount(pString->id) == 0 && (pString->op[0] != NULL || pString->op[1] != NULL || pString->op[2] != NULL)){
		printf("Error. Instruction '%s' at line %d can't have operands!\n", pString->instr_name, StringNum);
		return -1;
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
	instr = (instr | pString->id) << 1;
	instr = (instr | 1); // 32 bit flag
	*(int*)pOutBuf = instr;

	return 0;
}

int main(int argc, char* argv[])
{
	FILE* fp = 0;
	int	file_size = 0;
	char* pCode = 0;
	

	if(argc != 3){
		printf("Use VMAsm file.in file.out.\n");
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

	pAsm = (char*)malloc(file_size);
	fread(pAsm, 1, file_size, fp);
	fclose(fp);

	// clear \r
	for(int i = 0; i < file_size; i++){
		if(pAsm[i] == '\r'){
			pAsm[i] = ' ';
		}
	}

	ClearComments(pAsm, file_size);
	StringsNum = MakeStringsMap(pAsm, file_size);
	LabelsNum = MakeLabelsMap(pAsm, file_size);

	for(int i = 0; i < StringsNum; i++){
		ParseString(&pStrings[i]);
	}
	CalcSizeAndOffset(pStrings);

	pCode = (char*)malloc(pStrings[StringsNum - 1].offset + pStrings[StringsNum - 1].instruction_size);

	for(int i = 0; i < StringsNum; i++){
		CodeInstruction(pStrings, i, pLabels, pCode + pStrings[i].offset);
	}

	free(pAsm);

	fp = fopen(argv[2], "wb");
	if(fp == 0){
		printf("Can't open file %s for writing.\n", argv[2]);
		return 0;
	}

	fwrite(pCode, 1, pStrings[StringsNum - 1].offset + pStrings[StringsNum - 1].instruction_size, fp);
	fclose(fp);

	free(pCode);
	free(pLabels);
	free(pStrings);

	return 0;
}