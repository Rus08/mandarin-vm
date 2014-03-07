#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WebVM.h"
#include "Execute32Bit.h"
#include "Execute16Bit.h"


uint32_t CheckPass(struct VirtualMachine* pVM)
{
	uint32_t pc = 0;

	while(pc < (pVM->CodeSize - 4)){
		if((*(pVM->pCode + pc) & 0x01) == 1){
			// 32 bit
			uint32_t Instruction = *(uint32_t*)(pVM->pCode + pc);
			uint32_t id = (Instruction >> 1) & 127;

			if((pc % 4) != 0){
				assert(false);
				return VM_INVALID_INSTRUCTION_ALIGN;
			}

			if(id <= VM_XORI){
				// 3 operand instructions
				uint32_t last_int_flag = id & 1; // 0b00000001 get last integer flag
				if(last_int_flag == 0){
					uint32_t rep = (Instruction >> 8) & 31; // 0b00011111 get the repeat modifier
					uint32_t fop = (Instruction >> 13) & 31; // 0b00011111 get the first operand
					uint32_t sop = (Instruction >> 18) & 31; // 0b00011111 get the second operand
					uint32_t top = (Instruction >> 23) & 511; // get the third operand

					// repeat check
					if(fop + rep >= REGISTER_MAX || sop + rep >= REGISTER_MAX){
						// error
						assert(false);
						return VM_INVALID_REPEAT_MODIFIER;
					}
					if(last_int_flag == 0){
						if(top + rep >= REGISTER_MAX){
							// error
							assert(false);
							return VM_INVALID_REPEAT_MODIFIER;
						}
					}
				}
			}else if(id <= VM_NOTI){
				// 2 operand instructions
				uint32_t last_int_flag = id & 1; // 0b00000001 get last integer flag
				uint32_t rep, fop, sop;

				if(last_int_flag == 0){
					rep = (Instruction >> 8) & 31; // 0b00011111 get the repeat modifier
					fop = (Instruction >> 13) & 31; // 0b00011111 get the first operand
					sop = (Instruction >> 18); // 0b00011111 get the second operand
				}else{
					fop = (Instruction >> 8) & 31; // 0b00011111 get the first operand
					sop = (Instruction >> 13); // 0b00011111 get the second operand
				}

				if(last_int_flag == 0){
					if(fop + rep >= REGISTER_MAX || sop + rep >= REGISTER_MAX){
						// error
						assert(false);
						return VM_INVALID_REPEAT_MODIFIER;
					}
				}
			}else if(id <= VM_JMPI){
				// 1 operand instructions
				uint32_t last_int_flag = id & 1; // 0b00000001 get last integer flag
				uint32_t fop; // 0b00011111 get the first operand

				if(last_int_flag == 1){
					fop = (Instruction >> 8) & 16777215;
					if(((uint64_t)fop + 4) > pVM->CodeSize){
						assert(false);
						return VM_CODE_ACCESS_VIOLATION;
					}
					if((fop % 4) != 0){
						assert(false);
						return VM_INVALID_ADDRESS_ALIGN;
					}
				}
			}else if(id <= VM_STORELBI){
				// 2 operand memory instructions
				uint32_t last_int_flag = id & 1; // 0b00000001 get last integer flag
			
				uint32_t rep = (Instruction >> 8) & 31; // 0b00011111 get the repeat modifier
				uint32_t fop = (Instruction >> 13) & 31; // 0b00011111 get the first operand
				uint32_t sop = (Instruction >> 18); // 0b00011111 get the second operand
				
				// repeat check
				if(fop + rep >= REGISTER_MAX){
					// error
					assert(false);
					return VM_INVALID_REPEAT_MODIFIER;
				}
				if(last_int_flag == 0 && sop >= REGISTER_MAX){
					// error
					assert(false);
					return VM_INVALID_REPEAT_MODIFIER;
				}
			}else if(id <= VM_RET){
				uint32_t rep = (Instruction >> 8) & 31; // 0b00011111 get the repeat modifier
				uint32_t fop = (Instruction >> 13) & 31; // 0b00011111 get the first operand
				uint32_t sop = (Instruction >> 18); // 0b00011111 get the second operand
				if(id == VM_MOV){
					// repeat check
					if(fop + rep >= REGISTER_MAX || sop + rep >= REGISTER_MAX){
						// error
						assert(false);
						return VM_INVALID_REPEAT_MODIFIER;
					}
				}
				if(id == VM_COPY){
					// repeat check
					if(fop + rep >= REGISTER_MAX || sop >= REGISTER_MAX){
						// error
						assert(false);
						return VM_INVALID_REPEAT_MODIFIER;
					}
				}
			}
			pc = pc + 4;
		}else{
			// 16 bit
			pc = pc + 2;
		}
	}
	if(pc % 4 != 0){
		assert(false);
		return VM_INVALID_INSTRUCTION_ALIGN;
	}
	uint32_t id = (*(uint32_t*)(pVM->pCode + pc) >> 1) & 127;

	if((*(pVM->pCode + pc) & 0x01) != 0x01 || (id != VM_JMP && id != VM_JMPI && id != VM_RET)){
		assert(false);
		return VM_INVALID_LAST_INSTRUCTION;
	}

	return VM_OK;
}