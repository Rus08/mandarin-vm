#include <stdlib.h>
#include "WebVM.h"
#include "Execute32Bit.h"
#include "VMAsm/Instructions.h"
#include "SysCall.h"



uint32_t Execute32Bit(struct VirtualMachine* pVM, uint32_t Instruction)
{
	uint32_t id = (Instruction >> 1) & 63; // 0b00111111 get the instruction id

	if(id <= VM_XOR){
		// 3 operand instructions
		uint32_t rep = (Instruction >> 7) & 31; // 0b00011111 get the repeat modifier
		uint32_t fop = (Instruction >> 12) & 31; // 0b00011111 get the first operand
		uint32_t last_int_flag = (Instruction >> 17) & 1; // 0b00000001 get last integer flag
		uint32_t sop = (Instruction >> 18) & 31; // 0b00011111 get the second operand
		uint32_t top = (Instruction >> 23) & 511; // get the third operand
		//uint32_t result;

		// repeat check
		if(fop + rep >= REGISTER_MAX || sop + rep >= REGISTER_MAX){
			// error
			return VM_INVALID_REPEAT_MODIFIER;
		}
		if(last_int_flag == 0){
			if(top + rep >= REGISTER_MAX){
				// error
				return VM_INVALID_REPEAT_MODIFIER;
			}
		}

		switch(id){
			case VM_ADD:
			{
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
				if(last_int_flag == 0){
				// third operand is register
					top = top & 31;
					for(uint32_t i = 0; i <= rep; i++){
						pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] / pVM->Registers.r[top + i];
					}
				}else{
					for(uint32_t i = 0; i <= rep; i++){
						pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] / top;
					}
				}
				break;
			}
			case VM_MOD:
			{
				if(last_int_flag == 0){
				// third operand is register
					top = top & 31;
					for(uint32_t i = 0; i <= rep; i++){
						pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] % pVM->Registers.r[top + i];
					}
				}else{
					for(uint32_t i = 0; i <= rep; i++){
						pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] % top;
					}
				}
				break;
			}
			case VM_FADD:
			{
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
		};
		
	}else if(id <= VM_NOT){
		// 2 operand instructions
		uint32_t rep = (Instruction >> 7) & 31; // 0b00011111 get the repeat modifier
		uint32_t fop = (Instruction >> 12) & 31; // 0b00011111 get the first operand
		uint32_t last_int_flag = (Instruction >> 17) & 1; // 0b00000001 get last integer flag
		uint32_t sop = (Instruction >> 18) & 16383; // 0b00011111 get the second operand

		// repeat check
		if(fop + rep >= REGISTER_MAX){
			// error
			return VM_INVALID_REPEAT_MODIFIER;
		}
		if(last_int_flag == 0){
			if(sop + rep >= REGISTER_MAX){
				// error
				return VM_INVALID_REPEAT_MODIFIER;
			}
		}
		switch(id){
			case VM_CMP:
			{
				pVM->Registers.FLAGS = pVM->Registers.r[fop] == pVM->Registers.r[sop] ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
				pVM->Registers.FLAGS = pVM->Registers.r[fop] < pVM->Registers.r[sop] ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
			}
			break;
			case VM_NEG:
			{

			}
			break;
			case VM_LDI:
			{
				pVM->Registers.r[fop] = sop;
			}
			break;
			case VM_FCMP:
			{
				pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] == *(float*)&pVM->Registers.r[sop] ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
				pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] < *(float*)&pVM->Registers.r[sop] ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
			}
			break;
			case VM_NOT:
			{
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
		};
		
	}else if(id <= VM_SYSCALL){
		// 1 operand instructions
		uint32_t last_int_flag = (Instruction >> 7) & 1; // 0b00000001 get last integer flag
		uint32_t fop; // 0b00011111 get the first operand

		if(last_int_flag == 0){
			fop = pVM->Registers.r[(Instruction >> 8) & 31];
		}else{
			fop = (Instruction >> 8) & 16777215;
		}
		/*if(fop > pVM->CodeSize){
			return VM_CODE_ACCESS_VIOLATION;
		}*/
		
		switch(id){
			case VM_CALL:
			{
				if(pVM->CurrentStackTop == MAX_ALLOWED_STACK_SIZE){
					return VM_STACK_IS_TOO_BIG;
				}
				if(pVM->CurrentStackTop == pVM->CallStackSize){
					pVM->pCallStack = (Call*)realloc(pVM->pCallStack, pVM->CallStackSize + 32);
					pVM->CallStackSize = pVM->CallStackSize + 32;
				}

				pVM->CurrentStackTop = pVM->CurrentStackTop + 1;
				pVM->pCallStack[pVM->CurrentStackTop].regPC = pVM->Registers.PC + 4; // next instruction after call
				pVM->pCallStack[pVM->CurrentStackTop].regFLAGS = pVM->Registers.FLAGS;
				pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = (uint8_t*)malloc(LOCALMEMORY_START_SIZE);
				pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize = LOCALMEMORY_START_SIZE;

				pVM->Registers.PC = fop - 4; // compensate address increment
				break;
			}
			case VM_JEQ:
			{
				if((pVM->Registers.FLAGS & ZeroFlag) != 0){
					pVM->Registers.PC = fop - 4; // compensate address increment
				}
				break;
			}
			case VM_JNE:
			{
				if((pVM->Registers.FLAGS & ZeroFlag) == 0){
					pVM->Registers.PC = fop - 4; // compensate address increment
				}
				break;
			}
			case VM_JGR:
			{
				if((pVM->Registers.FLAGS & SignFlag) == 0 && (pVM->Registers.FLAGS & ZeroFlag) == 0){
					pVM->Registers.PC = fop - 4; // compensate address increment
				}
				break;
			}
			case VM_JLS:
			{
				if((pVM->Registers.FLAGS & SignFlag) != 0){
					pVM->Registers.PC = fop - 4; // compensate address increment
				}
				break;
			}
			case VM_JGE:
			{
				if((pVM->Registers.FLAGS & SignFlag) == 0){
					pVM->Registers.PC = fop - 4; // compensate address increment
				}
				break;
			}
			case VM_JLE:
			{
				if((pVM->Registers.FLAGS & SignFlag) != 0 || (pVM->Registers.FLAGS & ZeroFlag) != 0){
					pVM->Registers.PC = fop - 4; // compensate address increment
				}
				break;
			}
			case VM_JMP:
			{
				pVM->Registers.PC = fop - 4; // compensate address increment
				break;
			}
			case VM_SYSCALL:
			{
				return SysCall(pVM, fop);
				break;
			}
		};

	}else if(id <= VM_RET){
		// no operand instructions
		switch(id){
			case VM_RET:
			{
				if(pVM->CurrentStackTop == 0){
					// program exit
					return VM_COMPLETE;
				}
				free(pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory);
				pVM->Registers.PC = pVM->pCallStack[pVM->CurrentStackTop].regPC - 4; // compensate address increment
				pVM->Registers.FLAGS = pVM->pCallStack[pVM->CurrentStackTop].regFLAGS;
				pVM->CurrentStackTop = pVM->CurrentStackTop - 1;

				break;
			}
		};
	}else if(id <= VM_STORELB){
		// 2 operand memory instructions
		uint32_t rep = (Instruction >> 7) & 31; // 0b00011111 get the repeat modifier
		uint32_t fop = (Instruction >> 12) & 31; // 0b00011111 get the first operand
		uint32_t last_int_flag = (Instruction >> 17) & 1; // 0b00000001 get last integer flag
		uint32_t sop; // 0b00011111 get the second operand
		uint32_t localflag = (id - VM_LOAD) & 1; // odd is local
		uint32_t datasize = (rep + 1) << (2 - ((id - VM_LOAD) >> 1) % 3); // (rep + 1) * 4/2/1
		uint8_t* pData;

		// repeat check
		if(fop + rep >= REGISTER_MAX){
			// error
			return VM_INVALID_REPEAT_MODIFIER;
		}
		if(last_int_flag == 0){
			sop = pVM->Registers.r[(Instruction >> 18) & 31];
		}else{
			sop = (Instruction >> 18) & 16383;
		}
		if(localflag == 0){
			if((sop + datasize) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			pData = pVM->pGlobalMemory + sop;
		}else{
			if((sop + datasize) > pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			pData = pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory + sop;
		}
		if(id <= VM_LOADLB){
			// load
			switch(id){
				case VM_LOAD:
				case VM_LOADL:
				{
					for(uint32_t i = 0; i <= rep; i++){
						pVM->Registers.r[fop + i] = *(uint32_t*)(pData + 4 * i);
					}
				}
				break;
				case VM_LOADW:
				case VM_LOADLW:
				{
					for(uint32_t i = 0; i <= rep; i++){
						pVM->Registers.r[fop + i] = *(uint16_t*)(pData + 2 * i);
					}
				}
				break;
				case VM_LOADB:
				case VM_LOADLB:
				{
					for(uint32_t i = 0; i <= rep; i++){
						pVM->Registers.r[fop + i] = *(uint8_t*)(pData + i);
					}
				}
				break;
			};
		}else{
			// store
			switch(id){
				case VM_STORE:
				case VM_STOREL:
				{
					for(uint32_t i = 0; i <= rep; i++){
						*(uint32_t*)(pData + 4 * i) = pVM->Registers.r[fop + i];
					}
				}
				break;
				case VM_STOREW:
				case VM_STORELW:
				{
					for(uint32_t i = 0; i <= rep; i++){
						*(uint16_t*)(pData + 2 * i) = (uint16_t)pVM->Registers.r[fop + i];
					}
				}
				break;
				case VM_STOREB:
				case VM_STORELB:
				{
					for(uint32_t i = 0; i <= rep; i++){
						*(uint8_t*)(pData + i) = (uint8_t)pVM->Registers.r[fop + i];
					}
				}
				break;
			};
		}
		
	}/*else if(id <= VM_SYSCALL){
		// heavy instructions
		switch(id){
			
			break;
		};
	}*/else{
		return VM_INVALID_OPCODE;
	}

	return VM_OK;
}