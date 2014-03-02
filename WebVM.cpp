#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WebVM.h"
#include "Execute32Bit.h"
#include "Execute16Bit.h"

#ifdef STAT_COUNTERS
#include "VMAsm/Instructions.h"
#endif


uint32_t VMCreate(struct VirtualMachine* pVM, uint8_t* pCode, uint32_t CodeSize, uint8_t* pData, uint32_t DataSize,
				  uint32_t* pImport, uint32_t ImportSize, uint32_t* pExport, uint32_t ExportSize, HDC hDC)
{
	pVM->pCode = pCode;
	pVM->CodeSize = CodeSize;
	pVM->pGlobalMemory = pData;
	pVM->GlobalMemorySize = DataSize;
	if(pVM->GlobalMemorySize > MAX_ALLOWED_GLOBAL_MEMORY){
		return VM_DATA_SECTOR_IS_TOO_BIG;
	}
	// local memory
	pVM->pLocalMemory = (uint8_t*)malloc(LOCAL_MEMORY_START_SIZE);
	memset(pVM->pLocalMemory, 0, LOCAL_MEMORY_START_SIZE);
	pVM->pCurrentLocalMemory = pVM->pLocalMemory;
	pVM->LocalMemorySize = LOCAL_MEMORY_START_SIZE;
	pVM->CurrentLocalMemorySize = LOCAL_MEMORY_FRAME_START_SIZE;
	pVM->MaxNegativeOffset = 0;
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
	pVM->Registers.FLAGS = 0;
	pVM->DispatchFlag = false;
	pVM->hDC = hDC;
	memset(pVM->Callbacks, 0, sizeof(pVM->Callbacks));
#ifdef _WIN32
	QueryPerformanceCounter(&pVM->Timer);
#endif
#ifdef STAT_COUNTERS
	pVM->Count = 0;
	memset(pVM->ExecTable, 0, sizeof(pVM->ExecTable));
	memset(pVM->RegistersHit, 0, sizeof(pVM->RegistersHit));
#endif
	// start check pass
	uint32_t pc = 0;
	while(pc < (CodeSize - 4)){
		if((*(pCode + pc) & 0x01) == 1){
			// 32 bit
			uint32_t Instruction = *(uint32_t*)(pCode + pc);
			uint32_t id = (Instruction >> 1) & 63;

			if((pc % 4) != 0){
				VMDestroy(pVM);
				return VM_INVALID_INSTRUCTION_ALIGN;
			}

			if(id <= VM_XOR){
				// 3 operand instructions
				uint32_t rep = (Instruction >> 7) & 31; // 0b00011111 get the repeat modifier
				uint32_t fop = (Instruction >> 12) & 31; // 0b00011111 get the first operand
				uint32_t last_int_flag = (Instruction >> 17) & 1; // 0b00000001 get last integer flag
				uint32_t sop = (Instruction >> 18) & 31; // 0b00011111 get the second operand
				uint32_t top = (Instruction >> 23) & 511; // get the third operand

				// repeat check
				if(fop + rep >= REGISTER_MAX || sop + rep >= REGISTER_MAX){
					// error
					VMDestroy(pVM);
					return VM_INVALID_REPEAT_MODIFIER;
				}
				if(last_int_flag == 0){
					if(top + rep >= REGISTER_MAX){
						// error
						VMDestroy(pVM);
						return VM_INVALID_REPEAT_MODIFIER;
					}
				}
			}else if(id <= VM_NOT){
				// 2 operand instructions
				uint32_t rep = (Instruction >> 7) & 31; // 0b00011111 get the repeat modifier
				uint32_t fop = (Instruction >> 12) & 31; // 0b00011111 get the first operand
				uint32_t last_int_flag = (Instruction >> 17) & 1; // 0b00000001 get last integer flag
				uint32_t sop = (Instruction >> 18) & 16383; // 0b00011111 get the second operand

				// repeat check
				if(fop + rep >= REGISTER_MAX){
					// error
					VMDestroy(pVM);
					return VM_INVALID_REPEAT_MODIFIER;
				}
				if(last_int_flag == 0){
					if(sop + rep >= REGISTER_MAX){
						// error
						VMDestroy(pVM);
						return VM_INVALID_REPEAT_MODIFIER;
					}
				}
			}else if(id <= VM_JMP){
				// 1 operand instructions
				uint32_t last_int_flag = (Instruction >> 7) & 1; // 0b00000001 get last integer flag
				uint32_t fop; // 0b00011111 get the first operand

				if(last_int_flag == 1){
					fop = (Instruction >> 8) & 16777215;
					if(((uint64_t)fop + 4) > pVM->CodeSize){
						VMDestroy(pVM);
						return VM_CODE_ACCESS_VIOLATION;
					}
					if((fop % 4) != 0){
						VMDestroy(pVM);
						return VM_INVALID_ADDRESS_ALIGN;
					}
				}
			}else if(id <= VM_RET){

			}else if(id <= VM_STORELB){
				// 2 operand memory instructions
				uint32_t rep = (Instruction >> 7) & 31; // 0b00011111 get the repeat modifier
				uint32_t fop = (Instruction >> 12) & 31; // 0b00011111 get the first operand
				// repeat check
				if(fop + rep >= REGISTER_MAX){
					// error
					VMDestroy(pVM);
					return VM_INVALID_REPEAT_MODIFIER;
				}
			}
			pc = pc + 4;
		}else{
			// 16 bit
			pc = pc + 2;
		}
	}
	if(pc % 4 != 0){
		VMDestroy(pVM);
		return VM_INVALID_INSTRUCTION_ALIGN;
	}
	uint32_t id = (*(uint32_t*)(pCode + pc) >> 1) & 63;

	if((*(pCode + pc) & 0x01) != 0x01 || (id != VM_JMP && id != VM_RET)){
		VMDestroy(pVM);
		return VM_INVALID_LAST_INSTRUCTION;
	}

	return VM_OK;
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
	pVM->DispatchFlag = false;

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
#ifdef STAT_COUNTERS
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
#ifdef STAT_COUNTERS
			pVM->Count = pVM->Count + 1;
#endif
		}
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
		free(pVM->pRender);
	}
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
	free(pVM->pLocalMemory);
	pVM->pLocalMemory = 0;
	pVM->LocalMemorySize = 0;
	free(pVM->pCallStack);
	pVM->pCallStack = NULL;
	pVM->CallStackSize = 0;

	return VM_OK;
}

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
	fprintf(fp, "\r\nRegisters hit: \r\n");
	for(uint32_t i = 0; i < 32; i++){
		fprintf(fp, "r%.2d: %llu\r\n", i, pVM->RegistersHit[i]);
	}
	fclose(fp);
#endif
	
	return VM_OK;
}