#include <stdlib.h>
#include <string.h>
#include "WebVM.h"
#include "Execute32Bit.h"
#include "SysCall.h"
#include "Media\SysCallMedia.h"
#include "SysCallTable.h"



uint32_t SysCall(struct VirtualMachine* pVM, uint32_t SysCallId)
{

	if(SysCallId >= sizeof(SysCallTable) / sizeof(struct SysCall)){
		return VM_INVALID_SYSCALL;
	}

	return SysCallTable[SysCallId].pFunc(pVM);
}

uint32_t SysSetGlobalMemory(struct VirtualMachine* pVM)
{
	uint32_t mem_size = pVM->Registers.r[0];

	if(mem_size == 0){
		// free
		if(pVM->GlobalMemorySize > 0){
			free(pVM->pGlobalMemory);
			pVM->pGlobalMemory = 0;
			pVM->GlobalMemorySize = 0;
		}
		pVM->Registers.r[0] = 0;
		return VM_OK;
	}
	if(mem_size <= MAX_ALLOWED_GLOBAL_MEMORY){
		pVM->pGlobalMemory = (uint8_t*)realloc(pVM->pGlobalMemory, mem_size);
		if(pVM->pGlobalMemory != NULL){
			pVM->GlobalMemorySize = mem_size;
		}else{
			return VM_NOT_ENOUGH_MEMORY;
		}
	}
	pVM->Registers.r[0] = pVM->GlobalMemorySize;

	return VM_OK;
}

uint32_t SysSetLocalMemory(struct VirtualMachine* pVM)
{
	uint32_t mem_size = pVM->Registers.r[0];

	if(mem_size == 0){
		// return current size
		pVM->Registers.r[0] = pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize;
		return VM_OK;
	}
	if(mem_size <= MAX_ALLOWED_FRAME_LOCAL_MEMORY){
		if(IfAvailableLocalMemory(pVM, mem_size) == VM_OK){
			pVM->CurrentLocalMemorySize = mem_size;
		}
	}
	pVM->Registers.r[0] = pVM->CurrentLocalMemorySize;

	return VM_OK;
}

uint32_t SysRegisterCallback(struct VirtualMachine* pVM)
{
	if(pVM->Registers.r[0] >= (sizeof(pVM->Callbacks) / sizeof(uint32_t))){
		return VM_INVALID_CALLBACK;
	}
	if(((uint64_t)pVM->Registers.r[1] + 4) > pVM->CodeSize){
		return VM_CODE_ACCESS_VIOLATION;
	}
	pVM->Callbacks[pVM->Registers.r[0]] = pVM->Registers.r[1];

	return VM_OK;
}

uint32_t SysUnRegisterCallback(struct VirtualMachine* pVM)
{
	if(pVM->Registers.r[0] >= (sizeof(pVM->Callbacks) / sizeof(uint32_t))){
		return VM_INVALID_CALLBACK;
	}
	pVM->Callbacks[pVM->Registers.r[0]] = 0;

	return VM_OK;
}

uint32_t SysDispatchCallbacks(struct VirtualMachine* pVM)
{
	pVM->DispatchFlag = true;
	pVM->Registers.PC = pVM->Registers.PC + 4;

	return VM_DISPATCH;
}

uint32_t SysDebugOutput(struct VirtualMachine* pVM)
{
	uint32_t addr = pVM->Registers.r[0];
	uint32_t size = pVM->Registers.r[1];
	uint8_t* pTemp;

	if(((uint64_t)addr + size) > pVM->GlobalMemorySize){
		return VM_DATA_ACCESS_VIOLATION;
	}
	pTemp = (uint8_t*)malloc(size + 2);
	if(pTemp == NULL){
		return VM_NOT_ENOUGH_MEMORY;
	}
	strncpy((char*)pTemp, (char*)(pVM->pGlobalMemory + addr), size);
	pTemp[size] = '\n';
	pTemp[size + 1] = 0;
#ifdef _WIN32
	OutputDebugStringA((char*)pTemp);
#endif
	free(pTemp);

	return VM_OK;
}

uint32_t SysGetTimer(struct VirtualMachine* pVM)
{
	uint64_t Timer;
#ifdef _WIN32
	LARGE_INTEGER current, freq;
	QueryPerformanceCounter(&current);
	QueryPerformanceFrequency(&freq);

	Timer = (uint64_t)(1000000.0 * (double)(current.QuadPart - pVM->Timer.QuadPart) / (double)freq.QuadPart);

	pVM->Registers.r[0] = Timer & 0xffffffff;
	pVM->Registers.r[1] = (Timer >> 32);
#endif

	return VM_OK;
}

/*
	void Sleep(uint32_t ms)
	Sleeps defined amount of milliseconds
*/
uint32_t SysSleep(struct VirtualMachine* pVM)
{
#ifdef _WIN32
	Sleep(pVM->Registers.r[0]);
#endif

	return VM_OK;
}

uint32_t SysInteger64Operations(struct VirtualMachine* pVM)
{
	uint32_t id = pVM->Registers.r[0];
	uint32_t *op1 = &pVM->Registers.r[1]; // r1-r2, need check for Big Endian
	uint32_t *op2 = &pVM->Registers.r[3]; // r3-r4

	switch(id){
		case VM_INT64_ADD:
		{
			*(int64_t*)&pVM->Registers.r[0] = *(int64_t*)op1 + *(int64_t*)op2;
		}
		break;
		case VM_INT64_SUB:
		{
			*(int64_t*)&pVM->Registers.r[0] = *(int64_t*)op1 - *(int64_t*)op2;
		}
		break;
		case VM_INT64_MUL:
		{
			*(int64_t*)&pVM->Registers.r[0] = *(int64_t*)op1 * *(int64_t*)op2;
		}
		break;
		case VM_SINT64_DIV:
		{
			if(*(int64_t*)op2 == 0){
				return VM_DIVIDE_BY_ZERO;
			}
			*(int64_t*)&pVM->Registers.r[0] = *(int64_t*)op1 / *(int64_t*)op2;
		}
		break;
		case VM_UINT64_DIV:
		{
			if(*(uint64_t*)op2 == 0){
				return VM_DIVIDE_BY_ZERO;
			}
			*(uint64_t*)&pVM->Registers.r[0] = *(uint64_t*)op1 / *(uint64_t*)op2;
		}
		break;
		case VM_SINT64_MOD:
		{
			if(*(int64_t*)op2 == 0){
				return VM_DIVIDE_BY_ZERO;
			}
			*(int64_t*)&pVM->Registers.r[0] = *(int64_t*)op1 % *(int64_t*)op2;
		}
		break;
		case VM_UINT64_MOD:
		{
			if(*(uint64_t*)op2 == 0){
				return VM_DIVIDE_BY_ZERO;
			}
			*(uint64_t*)&pVM->Registers.r[0] = *(uint64_t*)op1 % *(uint64_t*)op2;
		}
		break;
		case VM_INT64_SHL:
		{
			*(int64_t*)&pVM->Registers.r[0] = *(int64_t*)op1 << *(int64_t*)op2;
		}
		break;
		case VM_SINT64_SHR:
		{
			*(int64_t*)&pVM->Registers.r[0] = *(int64_t*)op1 >> *(int64_t*)op2;
		}
		break;
		case VM_UINT64_SHR:
		{
			*(uint64_t*)&pVM->Registers.r[0] = *(uint64_t*)op1 >> *(uint64_t*)op2;
		}
		break;
		case VM_INT64_NEG:
		{
			*(int64_t*)&pVM->Registers.r[0] = -*(int64_t*)op1;
		}
		break;
		default:
			return VM_INVALID_SYSCALL;
	};

	return VM_OK;
}

uint32_t SysDoubleOperations(struct VirtualMachine* pVM)
{
	uint32_t id = pVM->Registers.r[0];
	double op1 = *(double*)&pVM->Registers.r[1]; // r1-r2, need check for Big Endian
	double op2 = *(double*)&pVM->Registers.r[3];

	switch(id){
		case VM_DOUBLE_ADD:
		{
			*(double*)&pVM->Registers.r[0] = op1 + op2;
		}
		break;
		case VM_DOUBLE_SUB:
		{
			*(double*)&pVM->Registers.r[0] = op1 - op2;
		}
		break;
		case VM_DOUBLE_MUL:
		{
			*(double*)&pVM->Registers.r[0] = op1 * op2;
		}
		break;
		case VM_DOUBLE_DIV:
		{
			*(double*)&pVM->Registers.r[0] = op1 / op2;
		}
		break;
		default:
			return VM_INVALID_SYSCALL;
	};

	return VM_OK;
}

