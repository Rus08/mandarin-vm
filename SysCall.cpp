#include <stdlib.h>
#include "WebVM.h"
#include "SysCall.h"
#include "SysCallTable.h"



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
		if(pVM->pGlobalMemory != NULL){
			free(pVM->pGlobalMemory);
			pVM->GlobalMemorySize = 0;
		}
		pVM->Registers.r[0] = 0;
		return VM_OK;
	}
	if(mem_size <= MAX_ALLOWED_GLOBAL_MEMORY){
		pVM->pGlobalMemory = (uint8_t*)realloc(pVM->pGlobalMemory, mem_size);
		if(pVM->pGlobalMemory != NULL){
			pVM->GlobalMemorySize = mem_size;
		}
	}
	pVM->Registers.r[0] = pVM->GlobalMemorySize;

	return VM_OK;
}