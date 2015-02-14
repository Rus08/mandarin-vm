#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define PTW32_STATIC_LIB
#include <pthread.h>
#include <windows.h>
#include <WebVM.h>
#include <Tools/VMAsmLib/VMAsmLib.h>

#include <time.h>

struct _String{
	char* pBegin;
	char* pEnd;
	char* pResult;
	char* pSC;
};

struct _String* pStrings = 0;
int StringsNum = 0;

enum ThreadFlags{
	THREAD_ALIVE,
	THREAD_COMPLETE,
	THREAD_MESSAGE,
	THREAD_ERROR,
	THREAD_EXIT,
};

pthread_t Threads[8];
char* ThreadBuffers[8] = { 0 };
int ThreadFlags[8] = { THREAD_ALIVE };

int ThreadsNum = 0;
bool not_break = false;
int	 error_count = 0;


uint32_t VMFileManagerInit(void)
{
	return VM_OK;
}

uint32_t VMFileManagerGetHandle(void)
{
	return VM_OK;
}
uint32_t VMFileManagerSetFlags(uint32_t handle, uint32_t flag_description, uint32_t flag_value)
{
	return VM_OK;
}
uint32_t VMFileManagerReadFile(uint32_t handle, struct FileStruct* pInfo)
{
	return VM_OK;
}
uint32_t VMFileManagerWriteFile(uint32_t handle, struct FileStruct* pInfo)
{
	return VM_OK;
}
uint32_t VMFileManagerCloseHandle(uint32_t handle)
{
	return VM_OK;
}

uint32_t VMFileManagerDeInit(void)
{
	return VM_OK;
}

uint32_t VMFontManagerInit(void)
{
	return VM_OK;
}

uint32_t VMFontManagerDeInit(void)
{
	return VM_OK;
}

static unsigned int DecodeHex(char* Op)
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

	pStrings = (struct _String*)malloc(sizeof(struct _String) * strings_num);
	memset(pStrings, 0, sizeof(struct _String) * strings_num);

	pBegin = pAsm;
	for(int i = 0; i < file_size; i++){	
		if(pAsm[i] == '\n'){
			pStrings[curr_str].pBegin = pBegin;
			pStrings[curr_str].pEnd = pAsm + i;
			pBegin = pAsm + i + 1; // need check
			curr_str = curr_str + 1;
		}
	}
	pStrings[strings_num - 1].pEnd = pAsm + file_size;
	StringsNum = strings_num;

	for(int i = 0; i < StringsNum; i++){
		char* pCurr = pStrings[i].pBegin;

		while(*pCurr != ','){
			pCurr = pCurr + 1;
		}
		*pCurr = 0;
		pStrings[i].pResult = pCurr + 1;
		pCurr = pCurr + 1;
		while(*pCurr != ','){
			pCurr = pCurr + 1;
		}
		*pCurr = 0;
		pStrings[i].pSC = pCurr + 1;
		pCurr = pCurr + 1;
		*pStrings[i].pEnd = 0;	
	}

	return strings_num;
}

int OnError(char* pCode, char* pData)
{
	if(pCode != NULL){
		free(pCode);
	}
	if(pData != NULL){
		free(pData);
	}
	
	return 0;
}

int PerformTest(char* pFileName, char* pResult, char* pSC, struct VirtualMachine* pVM, char* pMsgBuf)
{
	FILE* fp = 0;
	unsigned int SC = 0;
	char* pCode = 0, *pData = 0;
	int code_size = 0, data_size = 0;

	//system("del test.bin 2> nul");
	//system("del data.bin 2> nul");
	pMsgBuf = pMsgBuf + sprintf(pMsgBuf, "Performing %s: ", pFileName);
	if(strncmp(pFileName + strlen(pFileName) - 3, "asm", 3) != 0){
		// nullc
		char temp[256] = "";

		sprintf(temp, "nullcl -v test.asm %s", pFileName);
		system(temp);
		//system("VMAsm test.asm test.bin data.bin");
		SC = AsmFileToMemory("test.asm", &pCode, &code_size, &pData, &data_size);
	}else{
		// VMAsm
		char temp[256] = "";

		//sprintf(temp, "VMAsm %s test.bin data.bin", pFileName);
		//system(temp);
		SC = AsmFileToMemory(pFileName, &pCode, &code_size, &pData, &data_size);
	}
	if(SC == -1){
		OnError(pCode, pData);
		return 1;
	}

	SC = VMCreate(pVM, (uint8_t*)pCode, code_size, (uint8_t*)pData, data_size, NULL, 0, NULL, 0, NULL);
	if(SC != VM_OK){
		if(SC != DecodeHex(pSC)){
			pMsgBuf = pMsgBuf + sprintf(pMsgBuf, "Error, SC is: %d instead of %d\n", SC, DecodeHex(pSC));
			return 1;
		}
	}

	clock_t start = clock();

	SC = VMRun(pVM, 1000000);
	while(SC == VM_OK){
		SC = VMRun(pVM, 1000000);
	}

	clock_t range = clock() - start;
	pMsgBuf = pMsgBuf + sprintf(pMsgBuf, "%dms ", range);

	if(SC != DecodeHex(pSC)){
		pMsgBuf = pMsgBuf + sprintf(pMsgBuf, "Error, SC is: %d instead of %d\n", SC, DecodeHex(pSC));
		return 1;
	}else if(pVM->Registers.r[0] != DecodeHex(pResult)){
		pMsgBuf = pMsgBuf + sprintf(pMsgBuf, "Error, result is: 0x%x instead of 0x%x\n", pVM->Registers.r[0], DecodeHex(pResult));
		return 1;
	}
	sprintf(pMsgBuf, "OK\n");
	VMDestroy(pVM);

	return 0;
}

void* ThreadFunc(void* thread_num)
{
	struct VirtualMachine VM = { 0 };
	int SC = 0;

	for(int i = 0; i < StringsNum; i++){
		if((i % ThreadsNum) == (int)thread_num){
			SC = PerformTest(pStrings[i].pBegin, pStrings[i].pResult, pStrings[i].pSC, &VM, ThreadBuffers[(int)thread_num]);
			if(SC != 0){
				ThreadFlags[(int)thread_num] = THREAD_ERROR;
			}else{
				ThreadFlags[(int)thread_num] = THREAD_MESSAGE;
			}
			while(ThreadFlags[(int)thread_num] != THREAD_ALIVE){
				Sleep(5);
			}
		}
	}

	ThreadFlags[(int)thread_num] = THREAD_COMPLETE;
	return NULL;
}

int CancelThreads()
{
	for(int i = 0; i < ThreadsNum; i++){
		pthread_cancel(Threads[i]);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	FILE* fp = 0;
	int file_size = 0;
	char* pText = 0;
	clock_t start;
	int living_threads = 0;
	int flags_num = 0;


	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "-notbreak") == 0){
			not_break = true;
			flags_num = flags_num + 1;
			break;
		}
	}
	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "-t") == 0){
			if(i == (argc - 1)){
				printf("Error. Use -t N.\n");
				return 0;
			}
			ThreadsNum = atoi(argv[i + 1]);
			if(ThreadsNum < 0 || ThreadsNum > 8){
				printf("Error. Number of threads must be 0..8!\n");
				return 0;
			}
			flags_num = flags_num + 2;
		}
	}
	
	if((argc - flags_num) <= 3){
		if((argc - flags_num) == 1){
			fp = fopen("Tests\\list.txt", "rb");
		}else{
			fp = fopen(argv[1], "rb");
		}	

		if(fp == 0){
			if((argc - flags_num) == 1){
				printf("Tests\\list.txt not found!\n");
			}else{
				printf("%s not found!\n", argv[1]);
			}
			return 0;
		}

		fseek(fp, 0, SEEK_END);
		file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		pText = (char*)malloc(file_size + 1);
		fread(pText, 1, file_size, fp);

		fclose(fp);

		MakeStringsMap(pText, file_size);
		start = clock();

		if(ThreadsNum == 0){
			ThreadsNum = 1;
		}

		for(int i = 0; i < ThreadsNum; i++){
			ThreadBuffers[i] = (char*)malloc(4096);
			pthread_create(&Threads[i], NULL, ThreadFunc, (void*)i);
		}
		living_threads = ThreadsNum;

		while(living_threads != 0){
			for(int i = 0; i < ThreadsNum; i++){
				if(ThreadFlags[i] == THREAD_MESSAGE){
					printf("[%d]%s", i, ThreadBuffers[i]);
					ThreadFlags[i] = THREAD_ALIVE;
				}else if(ThreadFlags[i] == THREAD_ERROR){
					printf("[%d]%s", i, ThreadBuffers[i]);
					if(not_break == false){
						CancelThreads();
						living_threads = 0;
					}
					ThreadFlags[i] = THREAD_ALIVE;
				}else if(ThreadFlags[i] == THREAD_COMPLETE){
					living_threads = living_threads - 1;
					ThreadFlags[i] = THREAD_EXIT;
				}
			}
			Sleep(1);
		}		
			
	}else if((argc - flags_num) == 4){
		struct VirtualMachine VM;
		char temp[4096] = "";
		int SC = PerformTest(argv[1], argv[2], argv[3], &VM, temp);

		printf(temp);
		return SC;
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