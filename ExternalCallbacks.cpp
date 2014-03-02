#include <stdlib.h>
#include "WebVM.h"


uint32_t PerformCall(struct VirtualMachine* pVM)
{
	pVM->CurrentStackTop = pVM->CurrentStackTop + 1;

	if(pVM->CurrentStackTop == pVM->CallStackSize){
		pVM->CallStackSize = pVM->CallStackSize + 32;
		if(pVM->CallStackSize >= MAX_ALLOWED_STACK_SIZE){
			return VM_STACK_IS_TOO_BIG;
		}
		pVM->pCallStack = (Call*)realloc(pVM->pCallStack, sizeof(Call) * pVM->CallStackSize);
	}
	uint32_t LocalUsed = pVM->pCurrentLocalMemory - pVM->pLocalMemory + pVM->CurrentLocalMemorySize;
	if((LocalUsed + LOCAL_MEMORY_FRAME_START_SIZE) > pVM->LocalMemorySize){
		// need allocate more memory
		uint32_t new_size = pVM->LocalMemorySize + 64 * LOCAL_MEMORY_FRAME_START_SIZE;
		if(new_size > MAX_ALLOWED_LOCAL_MEMORY){
			return VM_NOT_ENOUGH_MEMORY;
		}
		pVM->pLocalMemory = (uint8_t*)realloc(pVM->pLocalMemory, new_size);
		pVM->LocalMemorySize = new_size;
	}

	pVM->pCurrentLocalMemory = pVM->pCurrentLocalMemory + pVM->CurrentLocalMemorySize;
	pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize = pVM->CurrentLocalMemorySize;
	pVM->CurrentLocalMemorySize = LOCAL_MEMORY_FRAME_START_SIZE;
	pVM->pCallStack[pVM->CurrentStackTop].regPC = pVM->Registers.PC + 4; // next instruction after call
	pVM->pCallStack[pVM->CurrentStackTop].regFLAGS = pVM->Registers.FLAGS;
	//pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = (uint8_t*)malloc(LOCALMEMORY_START_SIZE);
	return VM_OK;
}


uint32_t VMOnKeyDown(struct VirtualMachine* pVM, uint32_t Key)
{
	uint32_t SC;
	if(pVM->DispatchFlag == false){
		return VM_NOTINTIME_CALLBACK_CALL;
	}
	if(pVM->Callbacks[VM_ONKEYDOWN] == 0){
		return VM_CALLBACK_NOT_REGISTERED;
	}
	
	SC = PerformCall(pVM);
	if(SC != VM_OK){
		return SC;
	}

	*(uint32_t*)&pVM->pCurrentLocalMemory[0] = Key;
	pVM->Registers.PC = pVM->Callbacks[VM_ONKEYDOWN];

	return VM_OK;
}

uint32_t VMOnKeyUp(struct VirtualMachine* pVM, uint32_t Key)
{
	uint32_t SC;
	if(pVM->DispatchFlag == false){
		return VM_NOTINTIME_CALLBACK_CALL;
	}

	if(pVM->Callbacks[VM_ONKEYUP] == 0){
		return VM_CALLBACK_NOT_REGISTERED;
	}
	
	SC = PerformCall(pVM);
	if(SC != VM_OK){
		return SC;
	}

	*(uint32_t*)&pVM->pCurrentLocalMemory[0] = Key;
	pVM->Registers.PC = pVM->Callbacks[VM_ONKEYUP];

	return VM_OK;
}