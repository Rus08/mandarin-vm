#include <stdlib.h>
#include "WebVM.h"




uint32_t VMOnKeyDown(struct VirtualMachine* pVM, uint32_t Key)
{
	if(pVM->DispatchFlag == false){
		return VM_NOTINTIME_CALLBACK_CALL;
	}
	if(pVM->Callbacks[VM_ONKEYDOWN] == 0){
		return VM_CALLBACK_NOT_REGISTERED;
	}
	if(pVM->CurrentStackTop == MAX_ALLOWED_STACK_SIZE){
		return VM_STACK_IS_TOO_BIG;
	}

	if(pVM->CurrentStackTop == pVM->CallStackSize){
		pVM->pCallStack = (Call*)realloc(pVM->pCallStack, pVM->CallStackSize + 32);
		pVM->CallStackSize = pVM->CallStackSize + 32;
	}

	pVM->CurrentStackTop = pVM->CurrentStackTop + 1;
	pVM->pCallStack[pVM->CurrentStackTop].regPC = pVM->Registers.PC + 4;
	pVM->pCallStack[pVM->CurrentStackTop].regFLAGS = pVM->Registers.FLAGS;
	pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = (uint8_t*)malloc(LOCALMEMORY_START_SIZE);
	pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize = LOCALMEMORY_START_SIZE;

	*(uint32_t*)&pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory[0] = Key;
	pVM->Registers.PC = pVM->Callbacks[VM_ONKEYDOWN];

	return VM_OK;
}

uint32_t VMOnKeyUp(struct VirtualMachine* pVM, uint32_t Key)
{
	if(pVM->DispatchFlag == false){
		return VM_NOTINTIME_CALLBACK_CALL;
	}

	if(pVM->Callbacks[VM_ONKEYUP] == 0){
		return VM_CALLBACK_NOT_REGISTERED;
	}
	if(pVM->CurrentStackTop == MAX_ALLOWED_STACK_SIZE){
		return VM_STACK_IS_TOO_BIG;
	}

	if(pVM->CurrentStackTop == pVM->CallStackSize){
		pVM->pCallStack = (Call*)realloc(pVM->pCallStack, pVM->CallStackSize + 32);
		pVM->CallStackSize = pVM->CallStackSize + 32;
	}

	pVM->CurrentStackTop = pVM->CurrentStackTop + 1;
	pVM->pCallStack[pVM->CurrentStackTop].regPC = pVM->Registers.PC + 4;
	pVM->pCallStack[pVM->CurrentStackTop].regFLAGS = pVM->Registers.FLAGS;
	pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = (uint8_t*)malloc(LOCALMEMORY_START_SIZE);
	pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize = LOCALMEMORY_START_SIZE;

	*(uint32_t*)&pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory[0] = Key;
	pVM->Registers.PC = pVM->Callbacks[VM_ONKEYUP];

	return VM_OK;
}