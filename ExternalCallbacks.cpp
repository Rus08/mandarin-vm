#include <stdlib.h>
#include "WebVM.h"
#include "Execute32Bit.h"


uint32_t PerformCall(struct VirtualMachine* pVM)
{
	pVM->CurrentStackTop = pVM->CurrentStackTop + 1;
	CheckStackSize();

	MakeCall();
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

	//*(uint32_t*)&pVM->pCurrentLocalMemory[0] = Key; // this is broken now
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

	//*(uint32_t*)&pVM->pCurrentLocalMemory[0] = Key; // this is broken now
	pVM->Registers.PC = pVM->Callbacks[VM_ONKEYUP];

	return VM_OK;
}