#include <stdlib.h>
#include "WebVM.h"




uint32_t VMOnKeyDown(struct VirtualMachine* pVM, uint32_t Key)
{
	if(pVM->DispatchFlag == false){
		return VM_NOTINTIME_CALLBACK_CALL;
	}


	return VM_OK;
}

uint32_t VMOnKeyUp(struct VirtualMachine* pVM, uint32_t Key)
{
	if(pVM->DispatchFlag == false){
		return VM_NOTINTIME_CALLBACK_CALL;
	}


	return VM_OK;
}