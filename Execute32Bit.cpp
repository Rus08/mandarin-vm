#include <stdlib.h>
#include <string.h>
#include "WebVM.h"
#include "Execute32Bit.h"
#include "SysCall.h"
#include "SysCallTable.h"



//uint32_t Execute32Bit(struct VirtualMachine* pVM, uint32_t Instruction)
//{
uint32_t VMRun(struct VirtualMachine* pVM, uint32_t RunCount)
{
	//uint32_t RC; // Result Code
	pVM->DispatchFlag = false;

	for(uint32_t im = 0; im < RunCount; im++){
		uint32_t PC = pVM->Registers.PC;
		uint32_t Instruction = *(uint32_t*)&(pVM->pCode[PC]);
		uint32_t id = Instruction & 0xff; // 0b00111111 get the instruction id

		switch(id){
			I3Operands(VM_ADD, uint32_t, +, *(uint32_t*)&, false, 0);
			I3Operands(VM_SUB, uint32_t, -, *(uint32_t*)&, false, 0);
			I3Operands(VM_MUL, uint32_t, *, *(uint32_t*)&, false, 0);
			I3Operands(VM_DIV, uint32_t, /, *(uint32_t*)&, true, 0);
			I3Operands(VM_SDIV, int32_t, /, *(int32_t*)&, true, 8192);
			I3Operands(VM_MOD, uint32_t, %, *(uint32_t*)&, true, 0);
			I3Operands(VM_SMOD, int32_t, %, *(int32_t*)&, true, 8192);
			I3Operands(VM_FADD, float, +, *(int32_t*)&, false, 8192);
			I3Operands(VM_FSUB, float, -, *(int32_t*)&, false, 8192);
			I3Operands(VM_FMUL, float, *, *(int32_t*)&, false, 8192);
			I3Operands(VM_FDIV, float, /, *(int32_t*)&, false, 8192);
			I3Operands(VM_SHL, uint32_t, <<, *(uint32_t*)&, false, 0);
			I3Operands(VM_SHR, uint32_t, >>, *(uint32_t*)&, false, 0);
			I3Operands(VM_SAR, int32_t, >>, *(uint32_t*)&, false, 0);
			I3Operands(VM_AND, uint32_t, &, *(uint32_t*)&, false, 0);
			I3Operands(VM_OR, uint32_t, |, *(uint32_t*)&, false, 0);
			I3Operands(VM_XOR, uint32_t, ^, *(uint32_t*)&, false, 0);
			case VM_CMP:
			{
				I2Operands_Base();
				// second operand is register
				sop = pVM->Registers.r[sop];

				pVM->Registers.FLAGS = pVM->Registers.r[fop] == sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
				pVM->Registers.FLAGS = pVM->Registers.r[fop] < sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
				IDefaultEnd();
			}
			break;
			case VM_CMPI:
			{
				I2OperandsInt_Base();
				// second operand is integer
				pVM->Registers.FLAGS = pVM->Registers.r[fop] == sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
				pVM->Registers.FLAGS = pVM->Registers.r[fop] < sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
				IDefaultEnd();
			}
			break;
			case VM_SCMP:
			{
				I2Operands_Base();
				// second operand is register
				sop = pVM->Registers.r[sop];
				
				pVM->Registers.FLAGS = pVM->Registers.r[fop] == sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
				pVM->Registers.FLAGS = *(int32_t*)&pVM->Registers.r[fop] < *(int32_t*)&sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
				IDefaultEnd();
			}
			break;
			case VM_SCMPI:
			{
				I2OperandsInt_Base();
				// second operand is integer
				sop = sop - 262144;
				
				pVM->Registers.FLAGS = pVM->Registers.r[fop] == sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
				pVM->Registers.FLAGS = *(int32_t*)&pVM->Registers.r[fop] < *(int32_t*)&sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
				IDefaultEnd();
			}
			break;
			I2Operands(VM_NEG, uint32_t, -*(int32_t*)&, 0);
			case VM_LDI:
			{
				I2OperandsInt_Base();
				// second operand is integer
				pVM->Registers.r[fop] = sop;
				IDefaultEnd();
			}
			break;
			case VM_MOV:
			{
				I2Operands_Base();
				// second operand is register
				pVM->Registers.r[fop] = pVM->Registers.r[sop];
				IDefaultEnd();
			}
			break;
			case VM_MOVV:
			{
				I2Operands_Base();
				// second operand is register
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i];
				}
				IDefaultEnd();
			}
			break;
			case VM_COPY:
			{
				I2Operands_Base();
				// second operand is register
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop];
				}
				IDefaultEnd();
			}
			break;
			case VM_FCMP:
			{
				I2Operands_Base();
				// second operand is register
				pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] == *(float*)&pVM->Registers.r[sop] ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
				pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] < *(float*)&pVM->Registers.r[sop] ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
				IDefaultEnd();
			}
			break;
			case VM_FCMPI:
			{
				I2OperandsInt_Base();
				// second operand is integer
				sop = sop - 262144;
				pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] == (float)sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
				pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] < (float)*(int16_t*)&sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
				IDefaultEnd();
			}
			break;
			case VM_FTOI:
			{
				I2Operands_Base();
				// second operand is register
				pVM->Registers.r[fop] = (uint32_t)(int32_t)*(float*)&pVM->Registers.r[sop];
				IDefaultEnd();
			}
			break;
			case VM_FTOIV:
			{
				I2Operands_Base();
				// second operand is register
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = (uint32_t)(int32_t)*(float*)&pVM->Registers.r[sop + i];
				}
				IDefaultEnd();
			}
			break;
			I2Operands(VM_ITOF, float, (float)*(int32_t*)&, 262144);
			I2Operands(VM_NOT, uint32_t, ~, 0);
			case VM_CALL:
			{
				//	Save context and allocate local memory for new context
				// first operand is register
				I1Operand_Base();

				pVM->CurrentStackTop = pVM->CurrentStackTop + 1;
				CheckStackSize();
				if(IfAvailableLocalMemory(pVM, LOCAL_MEMORY_FRAME_START_SIZE) != VM_OK){
					assert(false);
					return VM_NOT_ENOUGH_MEMORY;
				}
				
				MakeCall();
				//pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = (uint8_t*)malloc(LOCALMEMORY_START_SIZE);
				pVM->Registers.PC = fop; // compensate address increment
			}
			break;
			case VM_CALLI:
			{
				//	Save context and allocate local memory for new context
				// first operand is integer
				I1OperandInt_Base();

				pVM->CurrentStackTop = pVM->CurrentStackTop + 1;
				CheckStackSize();
				if(IfAvailableLocalMemory(pVM, LOCAL_MEMORY_FRAME_START_SIZE) != VM_OK){
					assert(false);
					return VM_NOT_ENOUGH_MEMORY;
				}
				
				MakeCall();
				//pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = (uint8_t*)malloc(LOCALMEMORY_START_SIZE);
				pVM->Registers.PC = fop; // compensate address increment
			}
			break;
			case VM_JEQ:
			{
				// first operand is register
				if((pVM->Registers.FLAGS & ZeroFlag) != 0){
					I1Operand_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}
			}
			break;
			case VM_JEQI:
			{
				// first operand is integer
				if((pVM->Registers.FLAGS & ZeroFlag) != 0){
					I1OperandInt_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JNE:
			{
				// first operand is register
				if((pVM->Registers.FLAGS & ZeroFlag) == 0){
					I1Operand_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JNEI:
			{
				// first operand is integer
				if((pVM->Registers.FLAGS & ZeroFlag) == 0){
					I1OperandInt_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JGR:
			{
				// first operand is register
				if((pVM->Registers.FLAGS & SignFlag) == 0 && (pVM->Registers.FLAGS & ZeroFlag) == 0){
					I1Operand_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JGRI:
			{
				// first operand is integer
				if((pVM->Registers.FLAGS & SignFlag) == 0 && (pVM->Registers.FLAGS & ZeroFlag) == 0){
					I1OperandInt_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JLS:
			{
				// first operand is register
				if((pVM->Registers.FLAGS & SignFlag) != 0){
					I1Operand_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JLSI:
			{
				// first operand is integer
				if((pVM->Registers.FLAGS & SignFlag) != 0){
					I1OperandInt_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JGE:
			{
				// first operand is register
				if((pVM->Registers.FLAGS & SignFlag) == 0){
					I1Operand_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JGEI:
			{
				// first operand is integer
				if((pVM->Registers.FLAGS & SignFlag) == 0){
					I1OperandInt_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JLE:
			{
				// first operand is register
				if((pVM->Registers.FLAGS & SignFlag) != 0 || (pVM->Registers.FLAGS & ZeroFlag) != 0){
					I1Operand_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JLEI:
			{
				// first operand is integer
				if((pVM->Registers.FLAGS & SignFlag) != 0 || (pVM->Registers.FLAGS & ZeroFlag) != 0){
					I1OperandInt_Base();
					pVM->Registers.PC = fop; // compensate address increment
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JMP:
			{
				// first operand is register
				I1Operand_Base();
				pVM->Registers.PC = fop; // compensate address increment
			}
			break;
			case VM_JMPI:
			{
				// first operand is integer
				I1OperandInt_Base();
				pVM->Registers.PC = fop; // compensate address increment
			}
			break;
			case VM_SYSCALL:
			{
				I1OperandInt_Base();
				uint32_t SysCallId = fop;

				if(SysCallId >= sizeof(SysCallTable) / sizeof(struct SysCall)){
					assert(false);
					return VM_INVALID_SYSCALL;
				}
					
				uint32_t SC = SysCallTable[SysCallId].pFunc(pVM);
				if(SC != VM_OK){
					assert(SC == VM_DISPATCH);
					return SC;
				}
				IDefaultEnd();
			}
			break;
			case VM_RET:
			{
				// no operand instructions
				if(pVM->CurrentStackTop == 0){
					// program exit
					return VM_COMPLETE;
				}
				pVM->MaxNegativeOffset = pVM->MaxNegativeOffset + pVM->CurrentLocalMemorySize;
				pVM->pCurrentLocalMemory = pVM->pCurrentLocalMemory - pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize;
				pVM->CurrentLocalMemorySize = pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize;
				//pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = NULL; // unnecessary
				pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize = 0; // unnecessary
				pVM->Registers.PC = pVM->pCallStack[pVM->CurrentStackTop].regPC; // compensate address increment
				pVM->Registers.FLAGS = pVM->pCallStack[pVM->CurrentStackTop].regFLAGS;
				pVM->CurrentStackTop = pVM->CurrentStackTop - 1;

				break;
			}
			I2OperandsMem(VM_LOAD, uint32_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, true);
			I2OperandsMem(VM_LOADW, uint16_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, true);
			I2OperandsMem(VM_LOADB, uint8_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, true);
			I2OperandsMem(VM_STORE, uint32_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, false);
			I2OperandsMem(VM_STOREW, uint16_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, false);
			I2OperandsMem(VM_STOREB, uint8_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, false);
			I2OperandsMem(VM_LOADL, uint32_t, pVM->pCurrentLocalMemory, pVM->CurrentLocalMemorySize, true);
			I2OperandsMem(VM_LOADLW, uint16_t, pVM->pCurrentLocalMemory, pVM->CurrentLocalMemorySize, true);
			I2OperandsMem(VM_LOADLB, uint8_t, pVM->pCurrentLocalMemory, pVM->CurrentLocalMemorySize, true);
			I2OperandsMem(VM_STOREL, uint32_t, pVM->pCurrentLocalMemory, pVM->CurrentLocalMemorySize, false);
			I2OperandsMem(VM_STORELW, uint16_t, pVM->pCurrentLocalMemory, pVM->CurrentLocalMemorySize, false);
			I2OperandsMem(VM_STORELB, uint8_t, pVM->pCurrentLocalMemory, pVM->CurrentLocalMemorySize, false);
			case VM_MEMCPY:
			{
				// 3 operand instructions
				uint32_t flags = (Instruction >> 8) & 31; // 0b00011111 get the flags
				uint32_t fop = (Instruction >> 13) & 31; // 0b00011111 get the first operand
				uint32_t last_int_flag = flags & 4; // 0b00000001 get last integer flag
				uint32_t sop = (Instruction >> 18) & 31; // 0b00011111 get the second operand
				uint32_t top = (Instruction >> 23)/* & 511*/; // get the third operand
				uint8_t* pDst = 0;
				uint8_t* pSrc = 0;

				fop = pVM->Registers.r[fop];
				sop = pVM->Registers.r[sop];
				if(last_int_flag == 0){
					top = pVM->Registers.r[top];
				}

				if((flags & 0x1) != 0){ 
					// local memory
					if(*(int32_t*)&fop < pVM->MaxNegativeOffset ||(/*(uint64_t)*/*(int32_t*)&fop + top) > pVM->CurrentLocalMemorySize){
						assert(false);
						return VM_DATA_ACCESS_VIOLATION;
					}else{
						pDst = pVM->pCurrentLocalMemory + *(int32_t*)&fop;
					}
				}else{
					if(((uint64_t)fop + top) > pVM->GlobalMemorySize){
						assert(false);
						return VM_DATA_ACCESS_VIOLATION;
					}else{
						pDst = pVM->pGlobalMemory + fop;
					}
				}
				if((flags & 0x2) != 0){
					// local memory
					if(*(int32_t*)&sop < pVM->MaxNegativeOffset ||(/*(uint64_t)*/sop + top) > pVM->CurrentLocalMemorySize){
						assert(false);
						return VM_DATA_ACCESS_VIOLATION;
					}else{
						pSrc = pVM->pCurrentLocalMemory + *(int32_t*)&sop;
					}
				}else{
					if(((uint64_t)sop + top) > pVM->GlobalMemorySize){
						assert(false);
						return VM_DATA_ACCESS_VIOLATION;
					}else{
						pSrc = pVM->pGlobalMemory + sop;
					}
				}
				memcpy(pDst, pSrc, top);
				IDefaultEnd();
			}			
			break;
			default:
			{
				assert(false);
				return VM_INVALID_OPCODE;
			}
		};
		//pVM->Registers.PC = pVM->Registers.PC + 4;
#ifdef STAT_COUNTERS
		pVM->Count = pVM->Count + 1;
		pVM->ExecTable[id] = pVM->ExecTable[id] + 1;
#endif
	}

	return VM_OK;
}