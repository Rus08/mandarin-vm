#include "WebVM.h"
#include "Execute16Bit.h"



uint32_t Execute16Bit(struct VirtualMachine* pVM, uint16_t Instruction)
{
	uint32_t id = (Instruction >> 1) & 7;
	uint32_t fop = (Instruction >> 4) & 15;
	uint32_t sop = (Instruction >> 8) & 15;
	uint32_t top = (Instruction >> 12) & 15;

	switch(id){
		case VM_HADD:
		{
			pVM->Registers.r[fop] = pVM->Registers.r[sop] + pVM->Registers.r[top];
		}
		break;
		case VM_HSUB:
		{
			pVM->Registers.r[fop] = pVM->Registers.r[sop] - pVM->Registers.r[top];
		}
		break;
		case VM_HAND:
		{
			pVM->Registers.r[fop] = pVM->Registers.r[sop] & pVM->Registers.r[top];
		}
		break;
		case VM_HOR:
		{
			pVM->Registers.r[fop] = pVM->Registers.r[sop] | pVM->Registers.r[top];
		}
		break;
		case VM_HFADD:
		{
			*(float*)&pVM->Registers.r[fop] = *(float*)&pVM->Registers.r[sop] + *(float*)&pVM->Registers.r[top];
		}
		break;
		case VM_HFSUB:
		{
			*(float*)&pVM->Registers.r[fop] = *(float*)&pVM->Registers.r[sop] - *(float*)&pVM->Registers.r[top];
		}
		break;
		case VM_HFMUL:
		{
			*(float*)&pVM->Registers.r[fop] = *(float*)&pVM->Registers.r[sop] * *(float*)&pVM->Registers.r[top];
		}
		break;
		case VM_HNOP:
		{

		}
		break;
		default:
			assert(false);
	};

	return VM_OK;
}