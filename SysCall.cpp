#include <stdlib.h>
#include <string.h>
#include "WebVM.h"
#include "SysCall.h"
#include "SysCallTable.h"
#ifdef _WINDOWS
#include <windows.h>
#endif



uint32_t SysCall(struct VirtualMachine* pVM, uint32_t SysCallId)
{

	if(SysCallId >= sizeof(SysCallTable) / sizeof(struct SysCall)){
		return VM_INVALID_SYSCALL;
	}

	return SysCallTable[SysCallId].pFunc(pVM);
}

uint32_t SysSetGlobalMemory(struct VirtualMachine* pVM)
{
	uint32_t mem_size = pVM->Registers.r[0];

	if(mem_size == 0){
		// free
		if(pVM->GlobalMemorySize > 0){
			free(pVM->pGlobalMemory);
			pVM->pGlobalMemory = 0;
			pVM->GlobalMemorySize = 0;
		}
		pVM->Registers.r[0] = 0;
		return VM_OK;
	}
	if(mem_size <= MAX_ALLOWED_GLOBAL_MEMORY){
		pVM->pGlobalMemory = (uint8_t*)realloc(pVM->pGlobalMemory, mem_size);
		if(pVM->pGlobalMemory != NULL){
			pVM->GlobalMemorySize = mem_size;
		}else{
			return VM_NOT_ENOUGH_MEMORY;
		}
	}
	pVM->Registers.r[0] = pVM->GlobalMemorySize;

	return VM_OK;
}

uint32_t SysSetLocalMemory(struct VirtualMachine* pVM)
{
	uint32_t mem_size = pVM->Registers.r[0];

	if(mem_size == 0){
		// return current size
		pVM->Registers.r[0] = pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize;
		return VM_OK;
	}
	if(mem_size <= MAX_ALLOWED_LOCAL_MEMORY){
		pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = (uint8_t*)realloc(pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory, mem_size);
		pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize = mem_size;
	}
	pVM->Registers.r[0] = pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize;

	return VM_OK;
}

uint32_t SysRegisterCallback(struct VirtualMachine* pVM)
{
	if(pVM->Registers.r[0] >= (sizeof(pVM->Callbacks) / sizeof(uint32_t))){
		return VM_INVALID_CALLBACK;
	}
	if((pVM->Registers.r[1] + 4) > pVM->CodeSize){
		return VM_CODE_ACCESS_VIOLATION;
	}
	pVM->Callbacks[pVM->Registers.r[0]] = pVM->Registers.r[1];

	return VM_OK;
}

uint32_t SysUnRegisterCallback(struct VirtualMachine* pVM)
{
	if(pVM->Registers.r[0] >= (sizeof(pVM->Callbacks) / sizeof(uint32_t))){
		return VM_INVALID_CALLBACK;
	}
	pVM->Callbacks[pVM->Registers.r[0]] = 0;

	return VM_OK;
}

uint32_t SysDispatchCallbacks(struct VirtualMachine* pVM)
{
	pVM->DispatchFlag = true;

	return VM_DISPATCH;
}

uint32_t SysDebugOutput(struct VirtualMachine* pVM)
{
	uint32_t addr = pVM->Registers.r[0];
	uint32_t size = pVM->Registers.r[1];
	uint8_t* pTemp;

	if((uint64_t)(addr + size) > pVM->GlobalMemorySize){
		return VM_DATA_ACCESS_VIOLATION;
	}
	pTemp = (uint8_t*)malloc(size + 2);
	if(pTemp == NULL){
		return VM_NOT_ENOUGH_MEMORY;
	}
	strncpy((char*)pTemp, (char*)(pVM->pGlobalMemory + addr), size);
	pTemp[size] = '\n';
	pTemp[size + 1] = 0;
#ifdef _WINDOWS
	OutputDebugStringA((char*)pTemp);
#endif

	return VM_OK;
}