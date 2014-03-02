#include <stdlib.h>
#include <string.h>
#include "WebVM.h"
#include "Execute32Bit.h"
#include "SysCall.h"




uint32_t Execute32Bit(struct VirtualMachine* pVM, uint32_t Instruction)
{
	uint32_t id = (Instruction >> 1) & 63; // 0b00111111 get the instruction id

	switch(id){
		case VM_ADD:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] + pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] + top;
				}
			}
			break;
		}
		case VM_SUB:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] - pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] - top;
				}
			}
			break;
		}
		case VM_MUL:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] * pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] * top;
				}
			}
			break;
		}
		case VM_DIV:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					if(pVM->Registers.r[top + i] == 0){
						return VM_DIVIDE_BY_ZERO;
					}
					*(int32_t*)&pVM->Registers.r[fop + i] = *(int32_t*)&pVM->Registers.r[sop + i] / *(int32_t*)&pVM->Registers.r[top + i];
				}
			}else{
				if(top == 0){
					return VM_DIVIDE_BY_ZERO;
				}
				// sign extending
				struct { signed int op:9; } s;
				*(int32_t*)&top = s.op = top;
				for(uint32_t i = 0; i <= rep; i++){
					*(int32_t*)&pVM->Registers.r[fop + i] = *(int32_t*)&pVM->Registers.r[sop + i] / *(int32_t*)&top;
				}
			}
			break;
		}
		case VM_MOD:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					if(pVM->Registers.r[top + i] == 0){
						return VM_DIVIDE_BY_ZERO;
					}
					*(int32_t*)&pVM->Registers.r[fop + i] = *(int32_t*)&pVM->Registers.r[sop + i] % *(int32_t*)&pVM->Registers.r[top + i];
				}
			}else{
				if(top == 0){
					return VM_DIVIDE_BY_ZERO;
				}
				// sign extending
				struct { signed int op:9; } s;
				*(int32_t*)&top = s.op = top;
				for(uint32_t i = 0; i <= rep; i++){
					*(int32_t*)&pVM->Registers.r[fop + i] = *(int32_t*)&pVM->Registers.r[sop + i] % *(int32_t*)&top;
				}
			}
			break;
		}
		case VM_FADD:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] + *(float*)&pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] + (float)top;
				}
			}
			break;
		}
		case VM_FSUB:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] - *(float*)&pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] - (float)top;
				}
			}
			break;
		}
		case VM_FMUL:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] * *(float*)&pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] * (float)top;
				}
			}
			break;
		}
		case VM_FDIV:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] / *(float*)&pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] / (float)top;
				}
			}
			break;
		}
		case VM_SHL:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] << pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] << top;
				}
			}
			break;
		}
		case VM_SHR:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] >> pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] >> top;
				}
			}
			break;
		}
		case VM_AND:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] & pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] & top;
				}
			}
			break;
		}
		case VM_OR:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] | pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] | top;
				}
			}
			break;
		}
		case VM_XOR:
		{
			I3Operands_Base();
			

			if(last_int_flag == 0){
			// third operand is register
				top = top & 31;
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] ^ pVM->Registers.r[top + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] ^ top;
				}
			}
			break;
		}
	
		case VM_CMP:
		{
			I2Operands_Base();
			

			if(last_int_flag == 0){
				sop = sop & 31;
				sop = pVM->Registers.r[sop];
			}
			pVM->Registers.FLAGS = pVM->Registers.r[fop] == sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
			pVM->Registers.FLAGS = pVM->Registers.r[fop] < sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
		}
		break;
		case VM_SCMP:
		{
			I2Operands_Base();
			

			if(last_int_flag == 0){
				sop = sop & 31;
				sop = pVM->Registers.r[sop];
			}else{
				// sign extending
				struct { signed int op:14; } s;
				*(int32_t*)&sop = s.op = sop;
			}
			pVM->Registers.FLAGS = pVM->Registers.r[fop] == sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
			pVM->Registers.FLAGS = *(int32_t*)&pVM->Registers.r[fop] < *(int32_t*)&sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
		}
		break;
		case VM_NEG:
		{
			I2Operands_Base();
			

			if(last_int_flag == 0){
				sop = sop & 31;
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = -*(int32_t*)&pVM->Registers.r[sop + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = -*(int32_t*)&sop;
				}
			}
		}
		break;
		case VM_LDI:
		{
			I2Operands_Base();
			
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = sop;
			}
		}
		break;
		case VM_FCMP:
		{
			I2Operands_Base();
			

			if(last_int_flag == 0){
				sop = sop & 31;
				pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] == *(float*)&pVM->Registers.r[sop] ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
				pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] < *(float*)&pVM->Registers.r[sop] ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
			}else{
				pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] == (float)sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
				pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] < (float)sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
			}
		}
		break;
		case VM_FTOI:
		{
			I2Operands_Base();
			

			if(last_int_flag == 0){
			// second operand is register
				sop = sop & 31;
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = (uint32_t)(int32_t)*(float*)&pVM->Registers.r[sop + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = (uint32_t)(int32_t)*(float*)&sop;
				}
			}
		}
		break;
		case VM_ITOF:
		{
			I2Operands_Base();
			

			if(last_int_flag == 0){
			// second operand is register
				sop = sop & 31;
				for(uint32_t i = 0; i <= rep; i++){
					float b = (float)*(int32_t*)&pVM->Registers.r[sop + i];
					pVM->Registers.r[fop + i] = *(uint32_t*)&b;
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					// sign extending
					struct { signed int op:14; } s;
					int32_t iop = s.op = sop;
					float b = (float)iop;
					pVM->Registers.r[fop + i] = *(uint32_t*)&b;
				}
			}
		}
		break;
		case VM_NOT:
		{
			I2Operands_Base();
			

			if(last_int_flag == 0){
			// second operand is register
				sop = sop & 31;
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = ~pVM->Registers.r[sop + i];
				}
			}else{
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = ~sop;
				}
			}
		}
		break;
		case VM_CALL:
		{
			//	Save context and allocate local memory for new context
			I1Operand_Base();

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
				// need to allocate more memory
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

			pVM->Registers.PC = fop - 4; // compensate address increment
			break;
		}
		case VM_JEQ:
		{
			if((pVM->Registers.FLAGS & ZeroFlag) != 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
			break;
		}
		case VM_JNE:
		{
			if((pVM->Registers.FLAGS & ZeroFlag) == 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
			break;
		}
		case VM_JGR:
		{
			if((pVM->Registers.FLAGS & SignFlag) == 0 && (pVM->Registers.FLAGS & ZeroFlag) == 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
			break;
		}
		case VM_JLS:
		{
			if((pVM->Registers.FLAGS & SignFlag) != 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
			break;
		}
		case VM_JGE:
		{
			if((pVM->Registers.FLAGS & SignFlag) == 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
			break;
		}
		case VM_JLE:
		{
			if((pVM->Registers.FLAGS & SignFlag) != 0 || (pVM->Registers.FLAGS & ZeroFlag) != 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
			break;
		}
		case VM_JMP:
		{
			I1Operand_Base();

			pVM->Registers.PC = fop - 4; // compensate address increment
			break;
		}
		case VM_SYSCALL:
		{
			I1Operand_Base();

			return SysCall(pVM, fop);
			break;
		}
		case VM_RET:
		{
			// no operand instructions
			if(pVM->CurrentStackTop == 0){
				// program exit
				return VM_COMPLETE;
			}
			pVM->pCurrentLocalMemory = pVM->pCurrentLocalMemory - pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize;
			pVM->CurrentLocalMemorySize = pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize;
			//pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = NULL; // unnecessary
			pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize = 0; // unnecessary
			pVM->Registers.PC = pVM->pCallStack[pVM->CurrentStackTop].regPC - 4; // compensate address increment
			pVM->Registers.FLAGS = pVM->pCallStack[pVM->CurrentStackTop].regFLAGS;
			pVM->CurrentStackTop = pVM->CurrentStackTop - 1;

			break;
		}
		case VM_LOAD:
		{
			I2OperandsMem_Base();

			if(((uint64_t)sop + 4 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint32_t*)(pVM->pGlobalMemory + sop + 4 * i);
			}
		}
		break;
		case VM_LOADL:
		{
			I2OperandsMem_Base();
			
			if(((uint64_t)sop + 4 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint32_t*)(pVM->pCurrentLocalMemory + sop + 4 * i);
			}
		}
		break;
		case VM_LOADW:
		{
			I2OperandsMem_Base();
			
			if(((uint64_t)sop + 2 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint16_t*)(pVM->pGlobalMemory + sop + 2 * i);
			}
		}
		break;
		case VM_LOADLW:
		{
			I2OperandsMem_Base();
			
			if(((uint64_t)sop + 2 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint16_t*)(pVM->pCurrentLocalMemory + sop + 2 * i);
			}
		}
		break;
		case VM_LOADB:
		{
			I2OperandsMem_Base();
			
			if(((uint64_t)sop + 1 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint8_t*)(pVM->pGlobalMemory + sop + i);
			}

		}
		break;
		case VM_LOADLB:
		{
			I2OperandsMem_Base();
			
			if(((uint64_t)sop + 1 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint8_t*)(pVM->pCurrentLocalMemory + sop + i);
			}
		}
		break;
		case VM_STORE:
		{
			// store
			I2OperandsMem_Base();
			
			if(((uint64_t)sop + 4 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint32_t*)(pVM->pGlobalMemory + sop + 4 * i) = pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STOREL:
		{
			I2OperandsMem_Base();
			
			if(((uint64_t)sop + 4 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint32_t*)(pVM->pCurrentLocalMemory + sop + 4 * i) = pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STOREW:
		{
			I2OperandsMem_Base();
			
			if(((uint64_t)sop + 2 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint16_t*)(pVM->pGlobalMemory + sop + 2 * i) = (uint16_t)pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STORELW:
		{
			I2OperandsMem_Base();
			
			if(((uint64_t)sop + 2 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint16_t*)(pVM->pCurrentLocalMemory + sop + 2 * i) = (uint16_t)pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STOREB:
		{
			I2OperandsMem_Base();
			
			if(((uint64_t)sop + 1 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint8_t*)(pVM->pGlobalMemory + sop + i) = (uint8_t)pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STORELB:
		{
			I2OperandsMem_Base();
			
			if(((uint64_t)sop + 1 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint8_t*)(pVM->pCurrentLocalMemory + sop + i) = (uint8_t)pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_MEMCPY:
		{
			// 3 operand instructions
			uint32_t flags = (Instruction >> 7) & 31; // 0b00011111 get the flags
			uint32_t fop = (Instruction >> 12) & 31; // 0b00011111 get the first operand
			uint32_t last_int_flag = (Instruction >> 17) & 1; // 0b00000001 get last integer flag
			uint32_t sop = (Instruction >> 18) & 31; // 0b00011111 get the second operand
			uint32_t top = (Instruction >> 23) & 511; // get the third operand
			uint8_t* pDst = 0;
			uint8_t* pSrc = 0;

			fop = pVM->Registers.r[fop];
			sop = pVM->Registers.r[sop];
			if(last_int_flag == 0){
				top = top & 31;
				top = pVM->Registers.r[top];
			}

			if((flags & 0x1) != 0){ 
				// local memory
				if(((uint64_t)fop + top) >= pVM->CurrentLocalMemorySize){
					return VM_DATA_ACCESS_VIOLATION;
				}else{
					pDst = pVM->pCurrentLocalMemory + fop;
				}
			}else{
				if(((uint64_t)fop + top) >= pVM->GlobalMemorySize){
					return VM_DATA_ACCESS_VIOLATION;
				}else{
					pDst = pVM->pGlobalMemory + fop;
				}
			}
			if((flags & 0x2) != 0){
				// local memory
				if(((uint64_t)sop + top) >= pVM->CurrentLocalMemorySize){
					return VM_DATA_ACCESS_VIOLATION;
				}else{
					pSrc = pVM->pCurrentLocalMemory + sop;
				}
			}else{
				if(((uint64_t)sop + top) >= pVM->GlobalMemorySize){
					return VM_DATA_ACCESS_VIOLATION;
				}else{
					pSrc = pVM->pGlobalMemory + sop;
				}
			}
			memcpy(pDst, pSrc, top);
		}			
		break;
		default:
			return VM_INVALID_OPCODE;
		};

	return VM_OK;
}