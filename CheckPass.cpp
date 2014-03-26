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
		// 32 bit
		uint32_t Instruction = *(uint32_t*)(pVM->pCode + pc);
		uint32_t id = Instruction & 0xff;

		if((pc % 4) != 0){
			assert(false);
			return VM_INVALID_INSTRUCTION_ALIGN;
		}

		if(id <= VM_XORV){
			// 3 operand instructions
			uint32_t formnum = id % 3; // 0b00000001 get last integer flag
			if(formnum == 2){
				uint32_t rep = (Instruction >> 8) & 31; // 0b00011111 get the repeat modifier
				uint32_t fop = (Instruction >> 13) & 31; // 0b00011111 get the first operand
				uint32_t sop = (Instruction >> 18) & 31; // 0b00011111 get the second operand
				uint32_t top = (Instruction >> 23) & 511; // get the third operand
				// repeat check
				if(fop + rep >= REGISTER_MAX || sop + rep >= REGISTER_MAX || top + rep >= REGISTER_MAX){
					// error
					assert(false);
					return VM_INVALID_REPEAT_MODIFIER;
				}
			}				
		}else if(id <= VM_NOTV){
			// 2 operand instructions
			uint32_t formnum = id % 3; // 0b00000001 get last integer flag
			uint32_t rep, fop, sop;

			if(formnum == 2){
				rep = (Instruction >> 8) & 31; // 0b00011111 get the repeat modifier
				fop = (Instruction >> 13) & 31; // 0b00011111 get the first operand
				sop = (Instruction >> 18); // 0b00011111 get the second operand
			
				if(fop + rep >= REGISTER_MAX || sop + rep >= REGISTER_MAX){
					// error
					assert(false);
					return VM_INVALID_REPEAT_MODIFIER;
				}
			}
		}else if(id <= VM_JMPI){
			// 1 operand instructions
			uint32_t last_int_flag = (id - VM_CALL) & 1; // 0b00000001 get last integer flag
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
		}else if(id <= VM_STORELBVI){
			// 2 operand memory instructions
			uint32_t formnum = (id - VM_LOAD) % 4; // 0b00000001 get last integer flag
		
			uint32_t rep = (Instruction >> 8) & 31; // 0b00011111 get the repeat modifier
			uint32_t fop = (Instruction >> 13) & 31; // 0b00011111 get the first operand
			uint32_t sop = (Instruction >> 18); // 0b00011111 get the second operand
			
			if(formnum == 2 || formnum == 3){
				// repeat check
				if(fop + rep >= REGISTER_MAX){
					// error
					assert(false);
					return VM_INVALID_REPEAT_MODIFIER;
				}
				if(formnum == 2 && sop >= REGISTER_MAX){
					// error
					assert(false);
					return VM_INVALID_REPEAT_MODIFIER;
				}
			}
		}else if(id <= VM_RET){
			uint32_t rep = (Instruction >> 8) & 31; // 0b00011111 get the repeat modifier
			uint32_t fop = (Instruction >> 13) & 31; // 0b00011111 get the first operand
			uint32_t sop = (Instruction >> 18); // 0b00011111 get the second operand
			if(id == VM_MOVV){
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
	}
	if(pc % 4 != 0){
		assert(false);
		return VM_INVALID_INSTRUCTION_ALIGN;
	}
	uint32_t id = (*(uint32_t*)(pVM->pCode + pc)) & 0xff;

	if(id != VM_JMP && id != VM_JMPI && id != VM_RET){
		assert(false);
		return VM_INVALID_LAST_INSTRUCTION;
	}

	return VM_OK;
}