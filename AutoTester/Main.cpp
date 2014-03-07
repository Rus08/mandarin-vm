#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../WebVM.h"

#include <time.h>

struct String{
	char* pBegin;
	char* pEnd;
};

struct String* pStrings = 0;
int StringsNum = 0;

struct VirtualMachine VM;

unsigned int DecodeHex(char* Op)
{
	unsigned int op = 0;
	while(*Op == ' ' || *Op == '\t'){
		Op = Op + 1;
	}
	Op = Op + 2; // skip 0x
	while(*Op != 0){
		if('0' <= Op[0] && Op[0] <= '9'){
			op = (op << 4) + (Op[0] - '0');
		}else if('a' <= Op[0] && Op[0] <= 'f'){
			op = (op << 4) + (Op[0] - 'a' + 10);
		}else if('A' <= Op[0] && Op[0] <= 'F'){
			op = (op << 4) + (Op[0] - 'A' + 10);
		}else{
			break;
		}
		Op = Op + 1;
	}
	return op;
}

int MakeStringsMap(char* pAsm, int file_size)
{
	char* pBegin = 0;
	int strings_num = 0;
	int curr_str = 0;

	for(int i = 0; i < file_size; i++){
		if(pAsm[i] == '\n'){
			strings_num = strings_num + 1;
		}
	}

	pStrings = (struct String*)malloc(sizeof(struct String) * strings_num);
	memset(pStrings, 0, sizeof(struct String) * strings_num);

	pBegin = pAsm;
	for(int i = 0; i < file_size; i++){	
		if(pAsm[i] == '\n'){
			pStrings[curr_str].pBegin = pBegin;
			pStrings[curr_str].pEnd = pAsm + i;
			pBegin = pAsm + i + 1; // need check
			curr_str = curr_str + 1;
		}
	}
	StringsNum = strings_num;
	return strings_num;
}

int PerformTest(char* pFileName, char* pResult, char* pSC)
{
	FILE* fp = 0;
	unsigned int SC = 0;
	char* pCode = 0, *pData = 0;
	int code_size = 0, data_size = 0;

	system("del test.bin 2> nul");
	system("del data.bin 2> nul");
	printf("Performing %s: ", pFileName);
	if(strncmp(pFileName + strlen(pFileName) - 3, "asm", 3) != 0){
		// nullc
		char temp[256] = "";

		sprintf(temp, "nullcl -v test.asm %s", pFileName);
		system(temp);
		system("VMAsm test.asm test.bin data.bin");
	}else{
		// VMAsm
		char temp[256] = "";

		sprintf(temp, "VMAsm %s test.bin data.bin", pFileName);
		system(temp);
	}
	fp = fopen("test.bin", "rb");
	if(fp == 0){
		printf("test.bin not found!\n");
		return 2;
	}
	fseek(fp, 0, SEEK_END);
	code_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	pCode = (char*)malloc(code_size);
	fread(pCode, 1, code_size, fp);

	fclose(fp);

	fp = fopen("data.bin", "rb");
	if(fp != 0){
		fseek(fp, 0, SEEK_END);
		data_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		pData = (char*)malloc(data_size);
		fread(pData, 1, data_size, fp);

		fclose(fp);
	}

	SC = VMCreate(&VM, (uint8_t*)pCode, code_size, (uint8_t*)pData, data_size, NULL, 0, NULL, 0, NULL);
	if(SC != VM_OK){
		if(SC != DecodeHex(pSC)){
			printf("Error, SC is: %d instead of %d\n", SC, DecodeHex(pSC));
			return 1;
		}
	}

	clock_t start = clock();

	SC = VMRun(&VM, 1000000);
	while(SC == VM_OK){
		SC = VMRun(&VM, 1000000);
	}

	clock_t range = clock() - start;
	printf("%dms ", range);

	if(SC != DecodeHex(pSC)){
		printf("Error, SC is: %d instead of %d\n", SC, DecodeHex(pSC));
		return 1;
	}else if(VM.Registers.r[0] != DecodeHex(pResult)){
		printf("Error, result is: 0x%x instead of 0x%x\n", VM.Registers.r[0], DecodeHex(pResult));
		return 1;
	}
	printf("OK\n");
	VMDestroy(&VM);

	return 0;
}

int main(int argc, char* argv[])
{
	FILE* fp = 0;
	int file_size = 0;
	char* pText = 0;
	bool not_break = false;
	int	 error_count = 0;
	clock_t start;

	if(argc <= 3){
		if(argc <= 2){
			if(argc == 2 && strcmp(argv[1], "-notbreak") == 0){
				not_break = true;
				fp = fopen("Tests\\list.txt", "rb");
			}else{
				fp = fopen(argv[1], "rb");
			}
		}else{
			if(strcmp(argv[2], "-notbreak") == 0){
				not_break = true;
			}
			fp = fopen(argv[1], "rb");
		}
			

		if(fp == 0){
			if(argc <= 2){
				printf("Tests\\list.txt not found!\n");
			}else{
				printf("%s not found!\n", argv[1]);
			}
			return 0;
		}

		fseek(fp, 0, SEEK_END);
		file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		pText = (char*)malloc(file_size);
		fread(pText, 1, file_size, fp);

		fclose(fp);

		MakeStringsMap(pText, file_size);
		start = clock();

		for(int i = 0; i < StringsNum; i++){
			char* pFileName = pStrings[i].pBegin;
			char* pResult = 0;
			char* pSC = 0, *pCurr = pStrings[i].pBegin;
			int SC = 0;

			while(*pCurr != ','){
				pCurr = pCurr + 1;
			}
			*pCurr = 0;
			pResult = pCurr + 1;
			pCurr = pResult;
			while(*pCurr != ','){
				pCurr = pCurr + 1;
			}
			*pCurr = 0;
			pSC = pCurr + 1;
			pCurr = pSC;
			*pStrings[i].pEnd = 0;

			SC = PerformTest(pFileName, pResult, pSC);
			if(SC != 0){
				if(not_break == false){
					free(pStrings);
					return 1;
				}
				error_count = error_count + 1;
			}
		}
	}else if(argc == 4){
		return PerformTest(argv[1], argv[2], argv[3]);
	}else{
		printf("Use AutoTester filename result sc\n");
	}
	if(pStrings != NULL){
		free(pStrings);
	}
	clock_t end = clock();
	printf("Total errors found: %d Total execute time: %dms\n", error_count, end - start);
	return 0;
}