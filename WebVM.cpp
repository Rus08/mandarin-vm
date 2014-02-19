#include <stdlib.h>
#include <string.h>
#include "WebVM.h"
#include "Execute32Bit.h"
#include "Execute16Bit.h"


#define STACK_START_SIZE 16


uint32_t CreateVM(struct VirtualMachine* pVM, uint8_t* pCode, uint32_t CodeSize, uint8_t* pData, uint32_t DataSize,
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

	return 0;
}

uint32_t RunVM(struct VirtualMachine* pVM, uint32_t RunCount)
{
	uint32_t RC; // Result Code

	for(uint32_t i = 0; i < RunCount; i++){
		uint32_t PC = pVM->Registers.PC;
		// check for access violation
		if(PC + 4 > pVM->CodeSize){
			return VM_CODE_ACCESS_VIOLATION;
		}
		// check if instruction 32bit or 16bit
		if(pVM->pCode[PC] & 0x01){
			// 32bit
			if((pVM->Registers.FLAGS & Int16BitFlag) != 0){
				// Error here
				return -1;
			}
			RC = Execute32Bit(pVM, *(uint32_t*)&(pVM->pCode[PC]));
			if(RC != VM_OK){
				return RC;
			}
			pVM->Registers.PC = pVM->Registers.PC + 4;
		}else{
			// 16 bit
			pVM->Registers.FLAGS = (pVM->Registers.FLAGS & ~Int16BitFlag) | (~pVM->Registers.FLAGS & Int16BitFlag); // Not 16BitFlag

			Execute16Bit(pVM, *(uint16_t*)(pVM->pCode[PC]));
		}
	}

	return VM_OK;
}

uint32_t DestroyVM(struct VirtualMachine* pVM)
{
	// Clear code segment
	//free(pVM->pCode);
	pVM->pCode = NULL;
	pVM->CodeSize = 0;
	// Clear data segment
	if(pVM->pGlobalMemory != NULL){
		free(pVM->pGlobalMemory);
		pVM->pGlobalMemory = NULL;
		pVM->GlobalMemorySize = 0;
	}
	// Clear call stack and all call's local memory
	for(uint32_t i = 0; i < pVM->CallStackSize; i++){
		if(pVM->pCallStack[i].LocalMemory.pMemory != NULL){
			free(pVM->pCallStack[i].LocalMemory.pMemory);
		}
	}
	free(pVM->pCallStack);
	pVM->pCallStack = NULL;
	pVM->CallStackSize = 0;

	return 0;
}