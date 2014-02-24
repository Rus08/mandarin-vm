#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WebVM.h"
#include "Execute32Bit.h"
#include "Execute16Bit.h"
#ifdef _DEBUG
#include "VMAsm/Instructions.h"
#endif


uint32_t VMCreate(struct VirtualMachine* pVM, uint8_t* pCode, uint32_t CodeSize, uint8_t* pData, uint32_t DataSize,
				  uint32_t* pImport, uint32_t ImportSize, uint32_t* pExport, uint32_t ExportSize)
{
	pVM->pCode = pCode;
	pVM->CodeSize = CodeSize;
	pVM->pGlobalMemory = pData;
	pVM->GlobalMemorySize = DataSize;
	pVM->pCallStack = (struct Call*)malloc(sizeof(Call) * STACK_START_SIZE);
	memset(pVM->pCallStack, 0, sizeof(Call) * STACK_START_SIZE);
	pVM->CallStackSize = STACK_START_SIZE;
	pVM->CurrentStackTop = 0;
	pVM->pImport = pImport;
	pVM->ImportSize = ImportSize;
	pVM->pExport = pExport;
	pVM->ExportSize = ExportSize;
	memset(pVM->Registers.r, 0, sizeof(pVM->Registers.r));
	pVM->Registers.PC = 0;
	pVM->Registers.FLAGS = 0;
	pVM->DispatchFlag = false;
	memset(pVM->Callbacks, 0, sizeof(pVM->Callbacks));
#ifdef _DEBUG
	pVM->Count = 0;
	memset(pVM->ExecTable, 0, sizeof(pVM->ExecTable));
#endif

	return 0;
}

uint32_t VMRun(struct VirtualMachine* pVM, uint32_t RunCount)
{
	uint32_t RC; // Result Code
	// start check
	if(pVM->CodeSize < 2){
		return VM_CODE_ACCESS_VIOLATION;
	}
	if(pVM->pCode[0] & 0x01){
		if(pVM->CodeSize < 4){
			return VM_CODE_ACCESS_VIOLATION;
		}
	}

	for(uint32_t i = 0; i < RunCount; i++){
		uint32_t PC = pVM->Registers.PC;
		// check if instruction 32bit or 16bit
		if(pVM->pCode[PC] & 0x01){
			// 32bit
			/*if((pVM->Registers.FLAGS & Int16BitFlag) != 0){
				// Error here
				return -1;
			}*/
			RC = Execute32Bit(pVM, *(uint32_t*)&(pVM->pCode[PC]));
			if(RC != VM_OK){
				return RC;
			}
			pVM->Registers.PC = pVM->Registers.PC + 4;
			// check for access violation
			if((uint64_t)(PC + 4) > pVM->CodeSize){
				return VM_CODE_ACCESS_VIOLATION;
			}
#ifdef _DEBUG
			pVM->Count = pVM->Count + 1;
			uint32_t id = *(uint32_t*)&(pVM->pCode[PC]);
			id = (id >> 1) & 63;
			pVM->ExecTable[id] = pVM->ExecTable[id] + 1;
#endif
		}else{
			// 16 bit
			//pVM->Registers.FLAGS = (pVM->Registers.FLAGS & ~Int16BitFlag) | (~pVM->Registers.FLAGS & Int16BitFlag); // Not 16BitFlag
			Execute16Bit(pVM, *(uint16_t*)&(pVM->pCode[PC]));
			pVM->Registers.PC = pVM->Registers.PC + 2;
			if((uint64_t)(PC + 2) > pVM->CodeSize){
				return VM_CODE_ACCESS_VIOLATION;
			}
#ifdef _DEBUG
			pVM->Count = pVM->Count + 1;
#endif
		}
	}

	return VM_OK;
}

uint32_t VMDestroy(struct VirtualMachine* pVM)
{
	// Clear code segment
	if(pVM->pCode != NULL){
		free(pVM->pCode);
		pVM->pCode = NULL;
		pVM->CodeSize = 0;
	}
	// Clear data segment
	if(pVM->pGlobalMemory != NULL){
		free(pVM->pGlobalMemory);
		pVM->pGlobalMemory = NULL;
		pVM->GlobalMemorySize = 0;
	}
	// Clear call stack and all call's local memory
	for(uint32_t i = 0; i < pVM->CurrentStackTop; i++){
		if(pVM->pCallStack[i].LocalMemory.pMemory != NULL){
			free(pVM->pCallStack[i].LocalMemory.pMemory);
		}
	}
	free(pVM->pCallStack);
	pVM->pCallStack = NULL;
	pVM->CallStackSize = 0;

	return 0;
}

uint32_t VMPrintInfo(struct VirtualMachine* pVM, char* file_name)
{
#ifdef _DEBUG
	FILE* fp = 0;
	uint32_t id = 0;
	uint64_t max = 0;

	fp = fopen(file_name, "wb");
	if(fp == 0){
		return 0;
	}

	fprintf(fp, "Executed: %llu\r\n", pVM->Count);

	for(uint32_t b = 0; b < 64; b++){
		for(uint32_t i = 0; i < 64; i++){
			if(pVM->ExecTable[i] > max){
				max = pVM->ExecTable[i];
				id = i;
			}
		}
		if(max == 0){
			break;
		}
		fprintf(fp, "%s: %llu\r\n", Instructions[id].name, pVM->ExecTable[id]);
		pVM->ExecTable[id] = 0;
		max = 0;
	}
	fclose(fp);
#endif
	
	return VM_OK;
}