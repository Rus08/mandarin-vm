#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include "WebVM.h"
#include "Execute32Bit.h"

extern const uint32_t SysCallTableSize;

void Check3OpV(uint32_t Instruction, uint32_t pc, struct VirtualMachine* pVM)
{
	// reg-reg to reg with repeat, check 3rd operand and repeating
	I3OperandsV_Base();
	
	if(fop + rep >= REGISTER_MAX || sop + rep >= REGISTER_MAX || top + rep >= REGISTER_MAX){ // top + rep also checks top size for >= REGISTER_MAX
		assert(false);
		longjmp(pVM->CheckPassJmpBuf, VM_INVALID_REPEAT_MODIFIER);
	}
}

void Check3OpVS(uint32_t Instruction, uint32_t pc, struct VirtualMachine* pVM)
{
	// reg-reg to reg with repeat, single form, check repeating and 3rd operand
	I3OperandsV_Base();

	if(fop + rep >= REGISTER_MAX || sop + rep >= REGISTER_MAX){
		assert(false);
		longjmp(pVM->CheckPassJmpBuf, VM_INVALID_REPEAT_MODIFIER);
	}
}

void Check2OpV(uint32_t Instruction, uint32_t pc, struct VirtualMachine* pVM)
{
	I2OperandsV_Base();

	if(fop + rep >= REGISTER_MAX || sop + rep >= REGISTER_MAX){
		assert(false);
		longjmp(pVM->CheckPassJmpBuf, VM_INVALID_REPEAT_MODIFIER);
	}
}

void Check2OpVS(uint32_t Instruction, uint32_t pc, struct VirtualMachine* pVM)
{
	I2OperandsV_Base();

	if(fop + rep >= REGISTER_MAX){
		assert(false);
		longjmp(pVM->CheckPassJmpBuf, VM_INVALID_REPEAT_MODIFIER);
	}
}

void Check1OpI(uint32_t Instruction, uint32_t pc, struct VirtualMachine* pVM)
{
	I1OperandI_Base();

	*(int32_t*)&fop = (fop - 8388608) * 4; // unpack signed

	if(((uint64_t)pc + *(int32_t*)&fop + 4) > pVM->CodeSize){
		assert(false);
		longjmp(pVM->CheckPassJmpBuf, VM_CODE_ACCESS_VIOLATION);
	}
}

void Check2OpJMPI(uint32_t Instruction, uint32_t pc, struct VirtualMachine* pVM)
{
	I2Operands_Base();

	*(int32_t*)&sop = (sop - 262144) * 4; // unpack signed

	if(((uint64_t)pc + *(int32_t*)&sop + 4) > pVM->CodeSize){
		assert(false);
		longjmp(pVM->CheckPassJmpBuf, VM_CODE_ACCESS_VIOLATION);
	}
}

void CheckSYSCALL(uint32_t Instruction, uint32_t pc, struct VirtualMachine* pVM)
{
	I1OperandI_Base();

	if(fop >= SysCallTableSize){
		assert(false);
		longjmp(pVM->CheckPassJmpBuf, VM_INVALID_SYSCALL);
	}
}

void CheckBlank(uint32_t Instruction, uint32_t pc, struct VirtualMachine* pVM)
{
	
}

#define I3OPCHECK(name)\
	CheckBlank,\
	CheckBlank,\
	Check3OpV,\
	Check3OpVS

#define I3OPFCHECK(name)\
	CheckBlank,\
	Check3OpV,\
	Check3OpVS

#define I3OPCMPCHECK(name)\
	CheckBlank,\
	CheckBlank,\
	Check3OpV,\
	Check3OpVS

#define I3OPFCMPCHECK(name)\
	CheckBlank,\
	Check3OpV,\
	Check3OpVS

#define I2OPMEMCHECK(name)\
	CheckBlank,\
	CheckBlank,\
	Check2OpV,\
	Check2OpVS

void (*CheckFunc[])(uint32_t, uint32_t, struct VirtualMachine*) = {
	I3OPCHECK(VM_ADD),
	I3OPCHECK(VM_SUB),
	I3OPCHECK(VM_MUL),
	I3OPCHECK(VM_DIV),
	I3OPCHECK(VM_SDIV),
	I3OPCHECK(VM_MOD),
	I3OPCHECK(VM_SMOD),
	I3OPFCHECK(VM_FADD),
	I3OPFCHECK(VM_FSUB),
	I3OPFCHECK(VM_FMUL),
	I3OPFCHECK(VM_FDIV),
	I3OPCHECK(VM_SHL),
	I3OPCHECK(VM_SHR),
	I3OPCHECK(VM_SAR),
	I3OPCHECK(VM_AND),
	I3OPCHECK(VM_OR),
	I3OPCHECK(VM_XOR),
///////////////////
	I3OPCMPCHECK(VM_CMP_EQ),
	I3OPCMPCHECK(VM_CMP_NEQ),
	I3OPCMPCHECK(VM_CMP_GR),
	I3OPCMPCHECK(VM_CMP_LS),
	I3OPCMPCHECK(VM_CMP_GRE),
	I3OPCMPCHECK(VM_CMP_LSE),
	I3OPCMPCHECK(VM_SCMP_GR),
	I3OPCMPCHECK(VM_SCMP_LS),
	I3OPCMPCHECK(VM_SCMP_GRE),
	I3OPCMPCHECK(VM_SCMP_LSE),
	CheckBlank,		// VM_NEG
	Check2OpV,		// VM_NEGV
	CheckBlank,		// VM_LDI
	Check2OpVS,		// VM_LDIVS
	I3OPFCMPCHECK(VM_FCMP_EQ),
	I3OPFCMPCHECK(VM_FCMP_NEQ),
	I3OPFCMPCHECK(VM_FCMP_GR),
	I3OPFCMPCHECK(VM_FCMP_LS),
	I3OPFCMPCHECK(VM_FCMP_GRE),
	I3OPFCMPCHECK(VM_FCMP_LSE),
	I3OPFCMPCHECK(VM_FTOI),
	I3OPCHECK(VM_ITOF),
	CheckBlank,		// VM_NOT
	Check2OpV,		// VM_NOTV
	CheckBlank,		// VM_MOV
	Check2OpV,		// VM_MOVV
	Check2OpVS,		// VM_MOVVS
///////////////////
	CheckBlank,		// VM_CALL
	Check1OpI,		// VM_CALLI
	CheckBlank,		// VM_RET
	CheckBlank,		// VM_JEQ
	Check1OpI,		// VM_JEQI
	CheckBlank,		// VM_JNE
	Check1OpI,		// VM_JNEI
	CheckBlank,		// VM_JEQR
	Check2OpJMPI,	// VM_JEQRI
	Check2OpV,		// VM_JEQRV
	Check2OpVS,		// VM_JEQRVS
	CheckBlank,		// VM_JNER
	Check2OpJMPI,	// VM_JNERI
	Check2OpV,		// VM_JNERV
	Check2OpVS,		// VM_JNERVS
	CheckBlank,		// VM_JMP
	Check1OpI,		// VM_JMPI
	CheckSYSCALL,	// VM_SYSCALL
///////////////////
	I2OPMEMCHECK(VM_LOAD),
	I2OPMEMCHECK(VM_LOADW),
	I2OPMEMCHECK(VM_LOADB),
	I2OPMEMCHECK(VM_STORE),
	I2OPMEMCHECK(VM_STOREW),
	I2OPMEMCHECK(VM_STOREB),
////////////////////
	CheckBlank,		// VM_MEMCPY
	CheckBlank,		// VM_MEMCPYI	
};

uint32_t CheckPass(struct VirtualMachine* pVM)
{
	uint32_t pc = 0;
	uint32_t RC = 0; // return code

	if(pVM->CodeSize < 4){
		assert(false);
		return VM_CODE_ACCESS_VIOLATION;
	}

	RC = setjmp(pVM->CheckPassJmpBuf);
	if(RC != 0){
		return RC;
	}

	while(pc < (pVM->CodeSize - 4)){
		// 32 bit
		uint32_t Instruction = *(uint32_t*)(pVM->pCode + pc);
		uint32_t id = Instruction & 0xff;

		if(id >= sizeof(CheckFunc) / sizeof(CheckFunc[0])){
			assert(false);
			return VM_INVALID_OPCODE;
		}
		CheckFunc[id](Instruction, pc, pVM);
		
		pc = pc + 4;
	}
	
	uint32_t id = (*(uint32_t*)(pVM->pCode + pc)) & 0xff;

	if(id != VM_JMP && id != VM_JMPI && id != VM_RET){
		assert(false);
		return VM_INVALID_LAST_INSTRUCTION;
	}

	return VM_OK;
}