#include <stdlib.h>
#include <string.h>
#include "WebVM.h"
#include "Execute32Bit.h"
#include "SysCall.h"
#include "SysCallTable.h"

extern const uint32_t SysCallTableSize = sizeof(SysCallTable) / sizeof(struct SysCall);

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
			I3OperandsF(VM_FADD, float, +, *(int32_t*)&, false, 8192);
			I3OperandsF(VM_FSUB, float, -, *(int32_t*)&, false, 8192);
			I3OperandsF(VM_FMUL, float, *, *(int32_t*)&, false, 8192);
			I3OperandsF(VM_FDIV, float, /, *(int32_t*)&, false, 8192);
			I3Operands(VM_SHL, uint32_t, <<, *(uint32_t*)&, false, 0);
			I3Operands(VM_SHR, uint32_t, >>, *(uint32_t*)&, false, 0);
			I3Operands(VM_SAR, int32_t, >>, *(uint32_t*)&, false, 0);
			I3Operands(VM_AND, uint32_t, &, *(uint32_t*)&, false, 0);
			I3Operands(VM_OR, uint32_t, |, *(uint32_t*)&, false, 0);
			I3Operands(VM_XOR, uint32_t, ^, *(uint32_t*)&, false, 0);
			I2OperandsCMP(VM_CMP_EQ, uint32_t, ==, *(uint32_t*)&, 0);
			I2OperandsCMP(VM_CMP_NEQ, uint32_t, !=, *(uint32_t*)&, 0);
			I2OperandsCMP(VM_CMP_GR, uint32_t, >, *(uint32_t*)&, 0);
			I2OperandsCMP(VM_CMP_LS, uint32_t, <, *(uint32_t*)&, 0);
			I2OperandsCMP(VM_CMP_GRE, uint32_t, >=, *(uint32_t*)&, 0);
			I2OperandsCMP(VM_CMP_LSE, uint32_t, <=, *(uint32_t*)&, 0);
			I2OperandsCMP(VM_SCMP_GR, int32_t, >, *(int32_t*)&, 262144);
			I2OperandsCMP(VM_SCMP_LS, int32_t, <, *(int32_t*)&, 262144);
			I2OperandsCMP(VM_SCMP_GRE, int32_t, >=, *(int32_t*)&, 262144);
			I2OperandsCMP(VM_SCMP_LSE, int32_t, <=, *(int32_t*)&, 262144);
			case VM_NEG:
			{
				I2Operands_Base();
				// second operand is register
				*(int32_t*)&pVM->Registers.r[fop] = -(int32_t)pVM->Registers.r[sop]; // check this for type?
				IDefaultEnd();
			}
			break;
			case VM_NEGV:
			{
				I2OperandsV_Base();
				// second operand is register
				for(uint32_t i = 0; i <= rep; i++){
					*(int32_t*)&pVM->Registers.r[fop + i] = -(int32_t)pVM->Registers.r[sop + i]; // check this for type?
				}
				IDefaultEnd();
			}
			break;
			case VM_LDI:
			{
				I2OperandsI_Base();
				// second operand is integer
				pVM->Registers.r[fop] = sop;
				IDefaultEnd();
			}
			break;
			case VM_LDIVS:
			{
				I2OperandsV_Base();
				// second operand is integer
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = sop;
				}
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
				I2OperandsV_Base();
				// second operand is register
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i];
				}
				IDefaultEnd();
			}
			break;
			case VM_MOVVS:
			{
				I2OperandsV_Base();
				// second operand is register
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = pVM->Registers.r[sop];
				}
				IDefaultEnd();
			}
			break;
			I2OperandsFCMP(VM_FCMP_EQ, float, ==);
			I2OperandsFCMP(VM_FCMP_NEQ, float, !=);
			I2OperandsFCMP(VM_FCMP_GR, float, >);
			I2OperandsFCMP(VM_FCMP_LS, float, <);
			I2OperandsFCMP(VM_FCMP_GRE, float, >=);
			I2OperandsFCMP(VM_FCMP_LSE, float, <=);
			case VM_FTOI:
			{
				I2Operands_Base();
				// second operand is register
				*(int32_t*)&pVM->Registers.r[fop] = (int32_t)*(float*)&pVM->Registers.r[sop];
				IDefaultEnd();
			}
			break;
			case VM_FTOIV:
			{
				I2OperandsV_Base();
				// second operand is register
				for(uint32_t i = 0; i <= rep; i++){
					*(int32_t*)&pVM->Registers.r[fop + i] = (int32_t)*(float*)&pVM->Registers.r[sop + i];
				}
				IDefaultEnd();
			}
			break;
			case VM_FTOIVS:
			{
				I2OperandsV_Base();
				// second operand is register
				for(uint32_t i = 0; i <= rep; i++){
					*(int32_t*)&pVM->Registers.r[fop + i] = (int32_t)*(float*)&pVM->Registers.r[sop + i];
				}
				IDefaultEnd();
			}
			break;
			case VM_ITOF:
			{
				I2Operands_Base();
				// second operand is register
				*(float*)pVM->Registers.r[fop] = (float)*(int32_t*)&pVM->Registers.r[sop];
				IDefaultEnd();
			}
			break;
			case VM_ITOFI:
			{
				I2OperandsI_Base();
				// second operand is integer
				sop = sop - 262144;
				*(float*)pVM->Registers.r[fop] = (float)*(int32_t*)&sop;
				IDefaultEnd();
			}
			break;
			case VM_ITOFV:
			{
				I2OperandsV_Base();
				// second operand is register
				for(uint32_t i = 0; i <= rep; i++){
					*(float*)pVM->Registers.r[fop + i] = (float)*(int32_t*)&pVM->Registers.r[sop + i];
				}
				IDefaultEnd();
			}
			break;
			case VM_ITOFVS:
			{
				I2OperandsV_Base();
				// second operand is register
				for(uint32_t i = 0; i <= rep; i++){
					*(float*)pVM->Registers.r[fop + i] = (float)*(int32_t*)&pVM->Registers.r[sop];
				}
				IDefaultEnd();
			}
			break;
			case VM_NOT:
			{
				I2Operands_Base();
				// second operand is register
				pVM->Registers.r[fop] = ~pVM->Registers.r[sop];
				IDefaultEnd();
			}
			break;
			case VM_NOTV:
			{
				I2OperandsV_Base();
				// second operand is register
				for(uint32_t i = 0; i <= rep; i++){
					pVM->Registers.r[fop + i] = ~pVM->Registers.r[sop + i];
				}
				IDefaultEnd();
			}
			break;
			case VM_CALL:
			{
				// Save context
				// first operand is register
				I1Operand_Base();
				fop = pVM->Registers.r[fop];
				CodeAddressCheck(fop);

				pVM->CurrentStackTop = pVM->CurrentStackTop + 1;
				CheckStackSize();
				
				MakeCall();
				pVM->Registers.PC = fop; 
			}
			break;
			case VM_CALLI:
			{
				// Save context
				// first operand is integer
				I1OperandI_Base();

				*(int32_t*)&fop = (*(int32_t*)&fop - 8388608) * 4; // unpack signed
				//CodeAddressCheck(pVM->Registers.PC + *(int32_t*)&fop);

				pVM->CurrentStackTop = pVM->CurrentStackTop + 1;
				CheckStackSize();
				
				MakeCall();
				pVM->Registers.PC = pVM->Registers.PC + *(int32_t*)&fop;
			}
			break;
			case VM_JEQ:
			{
				// first operand is register
				if(pVM->Registers.REQ != 0){
					I1Operand_Base();
					fop = pVM->Registers.r[fop];
					CodeAddressCheck(fop);
					pVM->Registers.PC = fop;
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JEQI:
			{
				// first operand is integer
				if(pVM->Registers.REQ != 0){
					I1OperandI_Base();
					*(int32_t*)&fop = (*(int32_t*)&fop - 8388608) * 4; // unpack signed
					pVM->Registers.PC = pVM->Registers.PC + *(int32_t*)&fop;
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JNE:
			{
				// first operand is register
				if(pVM->Registers.REQ == 0){
					I1Operand_Base();
					fop = pVM->Registers.r[fop];
					CodeAddressCheck(fop);
					pVM->Registers.PC = fop;
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JNEI:
			{
				// first operand is integer
				if(pVM->Registers.REQ == 0){
					I1OperandI_Base();
					*(int32_t*)&fop = (*(int32_t*)&fop - 8388608) * 4; // unpack signed
					pVM->Registers.PC = pVM->Registers.PC + *(int32_t*)&fop;
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JEQR:
			{
				// second operand is register
				I2Operands_Base();
				
				if(fop != 0){
					CodeAddressCheck(sop);
					pVM->Registers.PC = sop;
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JEQRI:
			{
				// second operand is integer
				I2OperandsI_Base();
				if(fop != 0){
					*(int32_t*)&sop = (*(int32_t*)&sop - 262144) * 4; // unpack signed
					pVM->Registers.PC = pVM->Registers.PC + *(int32_t*)&sop;
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JNER:
			{
				// second operand is register
				I2Operands_Base();
				
				if(fop == 0){
					CodeAddressCheck(sop);
					pVM->Registers.PC = sop;
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JNERI:
			{
				// second operand is integer
				I2OperandsI_Base();
				if(fop == 0){
					*(int32_t*)&sop = (*(int32_t*)&sop - 262144) * 4; // unpack signed
					pVM->Registers.PC = pVM->Registers.PC + *(int32_t*)&sop;
				}else{
					IDefaultEnd();
				}
			}
			break;
			case VM_JEQRV:
			{
				// second operand is register
				I2OperandsV_Base();
				
				for(uint32_t i = 0; i <= rep; i++){
					if(pVM->Registers.r[fop + i] != 0){
						CodeAddressCheck(pVM->Registers.r[sop + i]);
						pVM->Registers.PC = pVM->Registers.r[sop + i];
						goto vm_jeqrv_exit;
					}
				}
				IDefaultEnd();
			}
			vm_jeqrv_exit:
			break;
			case VM_JEQRVS:
			{
				// second operand is register
				I2OperandsV_Base();
				for(uint32_t i = 0; i <= rep; i++){
					if(pVM->Registers.r[fop + i] != 0){
						CodeAddressCheck(pVM->Registers.r[sop]);
						pVM->Registers.PC = pVM->Registers.r[sop];
						goto vm_jeqrvs_exit;
					}
				}
				IDefaultEnd();
			}
			vm_jeqrvs_exit:
			break;
			case VM_JNERV:
			{
				// second operand is register
				I2OperandsV_Base();
				
				for(uint32_t i = 0; i <= rep; i++){
					if(pVM->Registers.r[fop + i] == 0){
						CodeAddressCheck(pVM->Registers.r[sop + i]);
						pVM->Registers.PC = pVM->Registers.r[sop + i];
						goto vm_jnerv_exit;
					}
				}
				IDefaultEnd();
			}
			vm_jnerv_exit:
			break;
			case VM_JNERVS:
			{
				// second operand is register
				I2OperandsV_Base();
				for(uint32_t i = 0; i <= rep; i++){
					if(pVM->Registers.r[fop + i] == 0){
						CodeAddressCheck(pVM->Registers.r[sop]);
						pVM->Registers.PC = pVM->Registers.r[sop];
						goto vm_jnervs_exit;
					}
				}
				IDefaultEnd();
			}
			vm_jnervs_exit:
			break;
			case VM_JMP:
			{
				// first operand is register
				I1Operand_Base();
				pVM->Registers.PC = fop;
			}
			break;
			case VM_JMPI:
			{
				// first operand is integer
				I1OperandI_Base();
				*(int32_t*)&fop = (*(int32_t*)&fop - 8388608) * 4; // unpack 24bit signed
				pVM->Registers.PC = pVM->Registers.PC + *(int32_t*)&fop;
			}
			break;
			case VM_SYSCALL:
			{
				I1OperandI_Base();
				uint32_t SysCallId = fop;
					
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
				pVM->Registers.PC = pVM->pCallStack[pVM->CurrentStackTop].regPC;
				pVM->CurrentStackTop = pVM->CurrentStackTop - 1;
				break;
			}
			I2OperandsMem(VM_LOAD, uint32_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, true);
			I2OperandsMem(VM_LOADW, uint16_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, true);
			I2OperandsMem(VM_LOADB, uint8_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, true);
			I2OperandsMem(VM_STORE, uint32_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, false);
			I2OperandsMem(VM_STOREW, uint16_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, false);
			I2OperandsMem(VM_STOREB, uint8_t, pVM->pGlobalMemory, pVM->GlobalMemorySize, false);
			case VM_MEMCPY:
			{
				// 3 operand instructions
				I3Operands_Base();
				uint8_t* pDst = 0;
				uint8_t* pSrc = 0;

				fop = pVM->Registers.r[fop];
				sop = pVM->Registers.r[sop];
				top = pVM->Registers.r[top];
				
				if(((uint64_t)fop + top) > pVM->GlobalMemorySize){
					assert(false);
					return VM_DATA_ACCESS_VIOLATION;
				}else{
					pDst = pVM->pGlobalMemory + fop;
				}
				
				if(((uint64_t)sop + top) > pVM->GlobalMemorySize){
					assert(false);
					return VM_DATA_ACCESS_VIOLATION;
				}else{
					pSrc = pVM->pGlobalMemory + sop;
				}
				
				memcpy(pDst, pSrc, top);
				IDefaultEnd();
			}			
			break;
			case VM_MEMCPYI: // optimize? this shares almost the same code with the vm_memcpy
			{
				// 3 operand instructions
				I3Operands_Base();
				uint8_t* pDst = 0;
				uint8_t* pSrc = 0;

				fop = pVM->Registers.r[fop];
				sop = pVM->Registers.r[sop];
				
				if(((uint64_t)fop + top) > pVM->GlobalMemorySize){
					assert(false);
					return VM_DATA_ACCESS_VIOLATION;
				}else{
					pDst = pVM->pGlobalMemory + fop;
				}
				
				if(((uint64_t)sop + top) > pVM->GlobalMemorySize){
					assert(false);
					return VM_DATA_ACCESS_VIOLATION;
				}else{
					pSrc = pVM->pGlobalMemory + sop;
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
#ifdef STAT_COUNTERS
		pVM->Count = pVM->Count + 1;
		pVM->ExecTable[id] = pVM->ExecTable[id] + 1;
#endif
	}

	return VM_OK;
}