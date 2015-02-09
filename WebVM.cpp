#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "WebVM.h"
#include "Execute32Bit.h"
#include "ThreadManager.h"
#include "SysCall.h"
#include "Render/RenderDefault.h"

#ifdef STAT_COUNTERS
#include "Tools/VMAsmLib/Instructions.h"
#endif

uint32_t CheckPass(struct VirtualMachine* pVM);


uint32_t VMCreate(struct VirtualMachine* pVM, uint8_t* pCode, uint32_t CodeSize, uint8_t* pData, uint32_t DataSize,
				  uint32_t* pImport, uint32_t ImportSize, uint32_t* pExport, uint32_t ExportSize, void* hDC)
{
	uint32_t SC;
	
	memset(pVM, 0, sizeof(struct VirtualMachine));
	pVM->pCode = pCode;
	pVM->CodeSize = CodeSize;
	pVM->pGlobalMemory = pData;
	pVM->GlobalMemorySize = DataSize;
	if(pVM->GlobalMemorySize > MAX_ALLOWED_GLOBAL_MEMORY){
		assert(false);
		return VM_DATA_SECTOR_IS_TOO_BIG;
	}
	// call stack
	pVM->pCallStack = (struct Call*)malloc(sizeof(Call) * STACK_START_SIZE);
	memset(pVM->pCallStack, 0, sizeof(Call) * STACK_START_SIZE);
	pVM->CallStackSize = STACK_START_SIZE;
	pVM->CurrentStackTop = 0;
	//
	pVM->pImport = pImport;
	pVM->ImportSize = ImportSize;
	pVM->pExport = pExport;
	pVM->ExportSize = ExportSize;
	memset(pVM->Registers.r, 0, sizeof(pVM->Registers.r));
	pVM->Registers.PC = 0;
	pVM->DispatchFlag = false;
	// threads
	SC = InitThreads(pVM);
	if(SC != VM_OK){
		assert(false);
		return SC;
	}
	pVM->hDC = hDC;
	memset(pVM->Callbacks, 0, sizeof(pVM->Callbacks));
#ifdef _WIN32
	QueryPerformanceCounter((LARGE_INTEGER*)&pVM->Timer);
#endif
#ifdef STAT_COUNTERS
	pVM->Count = 0;
	memset(pVM->ExecTable, 0, sizeof(pVM->ExecTable));
	memset(pVM->RegistersHit, 0, sizeof(pVM->RegistersHit));
#endif
	// start check pass
	SC = CheckPass(pVM);
	if(SC != VM_OK){
		assert(false);
		return SC;
	}

	return VM_OK;
}

uint32_t VMDestroy(struct VirtualMachine* pVM)
{
	// Clear render
	if(pVM->pRender != NULL){
		if(pVM->pRender->hRC != NULL){
			RenderDeInit(pVM->pRender, pVM->hDC);
		}
		vm_free(pVM->pRender);
	}
	// Clear threads
	DeInitThreads(pVM);
	// Clear code segment
	if(pVM->pCode != NULL){
		vm_free(pVM->pCode);
		pVM->pCode = NULL;
		pVM->CodeSize = 0;
	}
	// Clear data segment
	if(pVM->pGlobalMemory != NULL){
		vm_free(pVM->pGlobalMemory);
		pVM->pGlobalMemory = NULL;
		pVM->GlobalMemorySize = 0;
	}
	// Clear call stack
	vm_free(pVM->pCallStack);
	pVM->pCallStack = NULL;
	pVM->CallStackSize = 0;

	return VM_OK;
}

#ifndef CUSTOM_ALLOCATOR
void* vm_malloc(uint32_t size)
{
	return malloc(size);
}

void* vm_realloc(void* pMemory, uint32_t size)
{
	return realloc(pMemory, size);
}

void vm_free(void* pMemory)
{
	return free(pMemory);
}
#endif

uint32_t VMPrintInfo(struct VirtualMachine* pVM, char* file_name)
{
#ifdef STAT_COUNTERS
	FILE* fp = 0;
	uint32_t id = 0;
	uint64_t max = 0;

	fp = fopen(file_name, "wb");
	if(fp == 0){
		return 0;
	}

	fprintf(fp, "Executed: %llu\r\n", pVM->Count);

	for(uint32_t b = 0; b < 128; b++){
		for(uint32_t i = 0; i < 128; i++){
			if(pVM->ExecTable[i] > max){
				max = pVM->ExecTable[i];
				id = i;
			}
		}
		if(max == 0){
			break;
		}
		fprintf(fp, "%s: %llu\r\n", id < VM_LDI ? Instructions[id / 2].name : Instructions[id - VM_STORELBI / 2 - 1].name, pVM->ExecTable[id]);
		pVM->ExecTable[id] = 0;
		max = 0;
	}
	fprintf(fp, "\r\nRegisters hit: \r\n");
	for(uint32_t i = 0; i < 32; i++){
		fprintf(fp, "r%.2d: %llu\r\n", i, pVM->RegistersHit[i]);
	}
	fclose(fp);
#endif
	
	return VM_OK;
}