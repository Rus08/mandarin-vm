/*#include <stdlib.h>
#include <string.h>
#include "WebVM.h"
#include "Execute32Bit.h"
#include "SysCall.h"*/



//uint32_t Execute32Bit(struct VirtualMachine* pVM, uint32_t Instruction)
//{
	uint32_t id = (Instruction >> 1) & 127; // 0b00111111 get the instruction id

	switch(id){
		case VM_ADD:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] + pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_ADDI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			pVM->Registers.r[fop] = pVM->Registers.r[sop] + top;
		}
		break;
		case VM_SUB:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] - pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_SUBI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			pVM->Registers.r[fop] = pVM->Registers.r[sop] - top;
		}
		break;
		case VM_MUL:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] * pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_MULI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			pVM->Registers.r[fop] = pVM->Registers.r[sop] * top;
		}
		break;
		case VM_DIV:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				if(pVM->Registers.r[top + i] == 0){
					return VM_DIVIDE_BY_ZERO;
				}
				*(int32_t*)&pVM->Registers.r[fop + i] = *(int32_t*)&pVM->Registers.r[sop + i] / *(int32_t*)&pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_DIVI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			if(top == 0){
				return VM_DIVIDE_BY_ZERO;
			}
			// sign extending
			top = top - 8192;
			*(int32_t*)&pVM->Registers.r[fop] = *(int32_t*)&pVM->Registers.r[sop] / *(int32_t*)&top;
		}
		break;
		case VM_MOD:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				if(pVM->Registers.r[top + i] == 0){
					return VM_DIVIDE_BY_ZERO;
				}
				*(int32_t*)&pVM->Registers.r[fop + i] = *(int32_t*)&pVM->Registers.r[sop + i] % *(int32_t*)&pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_MODI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			if(top == 0){
				return VM_DIVIDE_BY_ZERO;
			}
			// sign extending
			top = top - 8192;
			*(int32_t*)&pVM->Registers.r[fop] = *(int32_t*)&pVM->Registers.r[sop] % *(int32_t*)&top;
		}
		break;
		case VM_FADD:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] + *(float*)&pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_FADDI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			*(float*)&pVM->Registers.r[fop] = *(float*)&pVM->Registers.r[sop] + (float)top;
		}
		break;
		case VM_FSUB:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] - *(float*)&pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_FSUBI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			*(float*)&pVM->Registers.r[fop] = *(float*)&pVM->Registers.r[sop] - (float)top;
		}
		break;
		case VM_FMUL:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] * *(float*)&pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_FMULI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			*(float*)&pVM->Registers.r[fop] = *(float*)&pVM->Registers.r[sop] * (float)top;
		}
		break;
		case VM_FDIV:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				*(float*)&pVM->Registers.r[fop + i] = *(float*)&pVM->Registers.r[sop + i] / *(float*)&pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_FDIVI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			*(float*)&pVM->Registers.r[fop] = *(float*)&pVM->Registers.r[sop] / (float)top;
		}
		break;
		case VM_SHL:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] << pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_SHLI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			pVM->Registers.r[fop] = pVM->Registers.r[sop] << top;
		}
		break;
		case VM_SHR:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] >> pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_SHRI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			pVM->Registers.r[fop] = pVM->Registers.r[sop] >> top;
		}
		break;
		case VM_AND:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] & pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_ANDI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			pVM->Registers.r[fop] = pVM->Registers.r[sop] & top;
		}
		break;
		case VM_OR:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] | pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_ORI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			pVM->Registers.r[fop] = pVM->Registers.r[sop] | top;
		}
		break;
		case VM_XOR:
		{
			I3Operands_Base();
			// third operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i] ^ pVM->Registers.r[top + i];
			}
		}
		break;
		case VM_XORI:
		{
			I3OperandsInt_Base();
			// third operand is integer
			pVM->Registers.r[fop] = pVM->Registers.r[sop] ^ top;
		}
		break;
		case VM_CMP:
		{
			I2Operands_Base();
			// second operand is register
			sop = pVM->Registers.r[sop];

			pVM->Registers.FLAGS = pVM->Registers.r[fop] == sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
			pVM->Registers.FLAGS = pVM->Registers.r[fop] < sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
		}
		break;
		case VM_CMPI:
		{
			I2OperandsInt_Base();
			// second operand is integer
			pVM->Registers.FLAGS = pVM->Registers.r[fop] == sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
			pVM->Registers.FLAGS = pVM->Registers.r[fop] < sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
		}
		break;
		case VM_SCMP:
		{
			I2Operands_Base();
			// second operand is register
			sop = pVM->Registers.r[sop];
			
			pVM->Registers.FLAGS = pVM->Registers.r[fop] == sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
			pVM->Registers.FLAGS = *(int32_t*)&pVM->Registers.r[fop] < *(int32_t*)&sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
		}
		break;
		case VM_SCMPI:
		{
			I2OperandsInt_Base();
			// second operand is integer
			sop = sop - 262144;
			
			pVM->Registers.FLAGS = pVM->Registers.r[fop] == sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
			pVM->Registers.FLAGS = *(int32_t*)&pVM->Registers.r[fop] < *(int32_t*)&sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
		}
		break;
		case VM_NEG:
		{
			I2Operands_Base();
			// second operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = -*(int32_t*)&pVM->Registers.r[sop + i];
			}
		}
		break;
		case VM_NEGI:
		{
			I2OperandsInt_Base();
			// second operand is integer
			pVM->Registers.r[fop] = -*(int32_t*)&sop;
		}
		break;
		case VM_LDI:
		{
			I2OperandsInt_Base();
			// second operand is integer
			pVM->Registers.r[fop] = sop;
		}
		break;
		case VM_MOV:
		{
			I2Operands_Base();
			// second operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = pVM->Registers.r[sop + i];
			}
		}
		break;
		case VM_COPY:
		{
			I2Operands_Base();
			// second operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = pVM->Registers.r[sop];
			}
		}
		break;
		case VM_FCMP:
		{
			I2Operands_Base();
			// second operand is register
			pVM->Registers.FLAGS = /**(float*)&*/pVM->Registers.r[fop] == /**(float*)&*/pVM->Registers.r[sop] ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
			pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] < *(float*)&pVM->Registers.r[sop] ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
		}
		break;
		case VM_FCMPI:
		{
			I2OperandsInt_Base();
			// second operand is integer
			sop = sop - 262144;
			pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] == (float)sop ? (pVM->Registers.FLAGS | ZeroFlag) : (pVM->Registers.FLAGS & ~ZeroFlag);
			pVM->Registers.FLAGS = *(float*)&pVM->Registers.r[fop] < (float)*(int16_t*)&sop ? (pVM->Registers.FLAGS | SignFlag) : (pVM->Registers.FLAGS & ~SignFlag);
		}
		break;
		case VM_FTOI:
		{
			I2Operands_Base();
			// second operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = (uint32_t)(int32_t)*(float*)&pVM->Registers.r[sop + i];
			}
		}
		break;
		case VM_FTOII:
		{
			I2OperandsInt_Base();
			// second operand is integer
			pVM->Registers.r[fop] = (uint32_t)(int32_t)*(float*)&sop;
		}
		break;
		case VM_ITOF:
		{
			I2Operands_Base();
			// second operand is register
			for(uint32_t i = 0; i <= rep; i++){
				float b = (float)*(int32_t*)&pVM->Registers.r[sop + i];
				pVM->Registers.r[fop + i] = *(uint32_t*)&b;
			}
		}
		break;
		case VM_ITOFI:
		{
			I2OperandsInt_Base();
			// second operand is integer
			float b = (float)((int32_t)sop - 262144);
			pVM->Registers.r[fop] = *(uint32_t*)&b;
		}
		break;
		case VM_NOT:
		{
			I2Operands_Base();
			// second operand is register
			for(uint32_t i = 0; i <= rep; i++){
				pVM->Registers.r[fop + i] = ~pVM->Registers.r[sop + i];
			}
		}
		break;
		case VM_NOTI:
		{
			I2OperandsInt_Base();
			// second operand is integer
			pVM->Registers.r[fop] = ~sop;
		}
		break;
		case VM_CALL:
		{
			//	Save context and allocate local memory for new context
			// first operand is register
			I1Operand_Base();

			pVM->CurrentStackTop = pVM->CurrentStackTop + 1;
			CheckStackSize();
			if(IfAvailableLocalMemory(pVM, LOCAL_MEMORY_FRAME_START_SIZE) != VM_OK){
				return VM_NOT_ENOUGH_MEMORY;
			}
			
			MakeCall();
			//pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = (uint8_t*)malloc(LOCALMEMORY_START_SIZE);
			pVM->Registers.PC = fop - 4; // compensate address increment
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
				return VM_NOT_ENOUGH_MEMORY;
			}
			
			MakeCall();
			//pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = (uint8_t*)malloc(LOCALMEMORY_START_SIZE);
			pVM->Registers.PC = fop - 4; // compensate address increment
		}
		break;
		case VM_JEQ:
		{
			// first operand is register
			if((pVM->Registers.FLAGS & ZeroFlag) != 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JEQI:
		{
			// first operand is integer
			if((pVM->Registers.FLAGS & ZeroFlag) != 0){
				I1OperandInt_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JNE:
		{
			// first operand is register
			if((pVM->Registers.FLAGS & ZeroFlag) == 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JNEI:
		{
			// first operand is integer
			if((pVM->Registers.FLAGS & ZeroFlag) == 0){
				I1OperandInt_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JGR:
		{
			// first operand is register
			if((pVM->Registers.FLAGS & SignFlag) == 0 && (pVM->Registers.FLAGS & ZeroFlag) == 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JGRI:
		{
			// first operand is integer
			if((pVM->Registers.FLAGS & SignFlag) == 0 && (pVM->Registers.FLAGS & ZeroFlag) == 0){
				I1OperandInt_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JLS:
		{
			// first operand is register
			if((pVM->Registers.FLAGS & SignFlag) != 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JLSI:
		{
			// first operand is integer
			if((pVM->Registers.FLAGS & SignFlag) != 0){
				I1OperandInt_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JGE:
		{
			// first operand is register
			if((pVM->Registers.FLAGS & SignFlag) == 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JGEI:
		{
			// first operand is integer
			if((pVM->Registers.FLAGS & SignFlag) == 0){
				I1OperandInt_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JLE:
		{
			// first operand is register
			if((pVM->Registers.FLAGS & SignFlag) != 0 || (pVM->Registers.FLAGS & ZeroFlag) != 0){
				I1Operand_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JLEI:
		{
			// first operand is integer
			if((pVM->Registers.FLAGS & SignFlag) != 0 || (pVM->Registers.FLAGS & ZeroFlag) != 0){
				I1OperandInt_Base();
				pVM->Registers.PC = fop - 4; // compensate address increment
			}
		}
		break;
		case VM_JMP:
		{
			// first operand is register
			I1Operand_Base();
			pVM->Registers.PC = fop - 4; // compensate address increment
		}
		break;
		case VM_JMPI:
		{
			// first operand is integer
			I1OperandInt_Base();
			pVM->Registers.PC = fop - 4; // compensate address increment
		}
		break;
		case VM_SYSCALL:
		{
			I1OperandInt_Base();
			uint32_t SysCallId = fop;

			if(SysCallId >= sizeof(SysCallTable) / sizeof(struct SysCall)){
				return VM_INVALID_SYSCALL;
			}
				
			uint32_t SC = SysCallTable[SysCallId].pFunc(pVM);
			if(SC != VM_OK){
				return SC;
			}
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
			pVM->Registers.PC = pVM->pCallStack[pVM->CurrentStackTop].regPC - 4; // compensate address increment
			pVM->Registers.FLAGS = pVM->pCallStack[pVM->CurrentStackTop].regFLAGS;
			pVM->CurrentStackTop = pVM->CurrentStackTop - 1;

			break;
		}
		case VM_LOAD:
		{
			I2OperandsMem_Base();
			// second operand is register
			if(((uint64_t)sop + 4 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint32_t*)(pVM->pGlobalMemory + sop + 4 * i);
			}
		}
		break;
		case VM_LOADI:
		{
			I2OperandsMemInt_Base();
			// second operand is integer
			if(((uint64_t)sop + 4 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint32_t*)(pVM->pGlobalMemory + sop + 4 * i);
			}
		}
		break;
		case VM_LOADW:
		{
			I2OperandsMem_Base();
			// second operand is register
			if(((uint64_t)sop + 2 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint16_t*)(pVM->pGlobalMemory + sop + 2 * i);
			}
		}
		break;
		case VM_LOADWI:
		{
			I2OperandsMemInt_Base();
			// second operand is integer
			if(((uint64_t)sop + 2 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint16_t*)(pVM->pGlobalMemory + sop + 2 * i);
			}
		}
		break;
		case VM_LOADB:
		{
			I2OperandsMem_Base();
			// second operand is register
			if(((uint64_t)sop + 1 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint8_t*)(pVM->pGlobalMemory + sop + i);
			}
		}
		break;
		case VM_LOADBI:
		{
			I2OperandsMemInt_Base();
			// second operand is integer
			if(((uint64_t)sop + 1 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint8_t*)(pVM->pGlobalMemory + sop + i);
			}
		}
		break;
		case VM_STORE:
		{	// store
			I2OperandsMem_Base();
			// second operand is register
			if(((uint64_t)sop + 4 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint32_t*)(pVM->pGlobalMemory + sop + 4 * i) = pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STOREI:
		{	// store
			I2OperandsMemInt_Base();
			// second operand is integer
			if(((uint64_t)sop + 4 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint32_t*)(pVM->pGlobalMemory + sop + 4 * i) = pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STOREW:
		{
			I2OperandsMem_Base();
			// second operand is register
			if(((uint64_t)sop + 2 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint16_t*)(pVM->pGlobalMemory + sop + 2 * i) = (uint16_t)pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STOREWI:
		{
			I2OperandsMemInt_Base();
			// second operand is integer
			if(((uint64_t)sop + 2 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint16_t*)(pVM->pGlobalMemory + sop + 2 * i) = (uint16_t)pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STOREB:
		{
			I2OperandsMem_Base();
			// second operand is register
			if(((uint64_t)sop + 1 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint8_t*)(pVM->pGlobalMemory + sop + i) = (uint8_t)pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STOREBI:
		{
			I2OperandsMemInt_Base();
			// second operand is integer
			if(((uint64_t)sop + 1 * rep) > pVM->GlobalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint8_t*)(pVM->pGlobalMemory + sop + i) = (uint8_t)pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_LOADL:
		{
			I2OperandsMem_Base();
			// second operand is register
			if((/*(uint64_t)*/sop + 4 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint32_t*)(pVM->pCurrentLocalMemory + sop + 4 * i);
			}
		}
		break;
		case VM_LOADLI:
		{
			I2OperandsMemInt_Base();
			// second operand is integer
			if((/*(uint64_t)*/sop + 4 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint32_t*)(pVM->pCurrentLocalMemory + sop + 4 * i);
			}
		}
		break;
		case VM_LOADLW:
		{
			I2OperandsMem_Base();
			// second operand is register
			if((/*(uint64_t)*/sop + 2 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint16_t*)(pVM->pCurrentLocalMemory + sop + 2 * i);
			}
		}
		break;
		case VM_LOADLWI:
		{
			I2OperandsMemInt_Base();
			// second operand is integer
			if((/*(uint64_t)*/sop + 2 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint16_t*)(pVM->pCurrentLocalMemory + sop + 2 * i);
			}
		}
		break;
		case VM_LOADLB:
		{
			I2OperandsMem_Base();
			// second operand is register
			if((/*(uint64_t)*/sop + 1 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint8_t*)(pVM->pCurrentLocalMemory + sop + i);
			}
		}
		break;
		case VM_LOADLBI:
		{
			I2OperandsMemInt_Base();
			// second operand is integer
			if((/*(uint64_t)*/sop + 1 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				pVM->Registers.r[fop + i] = *(uint8_t*)(pVM->pCurrentLocalMemory + sop + i);
			}
		}
		break;
		case VM_STOREL:
		{
			I2OperandsMem_Base();
			// second operand is register
			if((/*(uint64_t)*/sop + 4 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint32_t*)(pVM->pCurrentLocalMemory + *(int32_t*)&sop + 4 * i) = pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STORELI:
		{
			I2OperandsMemInt_Base();
			// second operand is integer
			if((/*(uint64_t)*/sop + 4 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint32_t*)(pVM->pCurrentLocalMemory + sop + 4 * i) = pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STORELW:
		{
			I2OperandsMem_Base();
			// second operand is register
			if((/*(uint64_t)*/sop + 2 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint16_t*)(pVM->pCurrentLocalMemory + sop + 2 * i) = (uint16_t)pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STORELWI:
		{
			I2OperandsMemInt_Base();
			// second operand is integer
			if((/*(uint64_t)*/sop + 2 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint16_t*)(pVM->pCurrentLocalMemory + sop + 2 * i) = (uint16_t)pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STORELB:
		{
			I2OperandsMem_Base();
			// second operand is register
			if((/*(uint64_t)*/sop + 1 * rep) > pVM->CurrentLocalMemorySize){
				return VM_DATA_ACCESS_VIOLATION;
			}
			for(uint32_t i = 0; i < rep; i++){
				*(uint8_t*)(pVM->pCurrentLocalMemory + sop + i) = (uint8_t)pVM->Registers.r[fop + i];
			}
		}
		break;
		case VM_STORELBI:
		{
			I2OperandsMemInt_Base();
			// second operand is integer
			if((/*(uint64_t)*/sop + 1 * rep) > pVM->CurrentLocalMemorySize){
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

//	return VM_OK;
//}