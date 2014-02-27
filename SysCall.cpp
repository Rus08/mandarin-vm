#include <stdlib.h>
#include <string.h>
#include "WebVM.h"
#include "SysCall.h"
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
	if(mem_size <= MAX_ALLOWED_LOCAL_MEMORY){
		pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory = (uint8_t*)realloc(pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.pMemory, mem_size);
		pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize = mem_size;
	}
	pVM->Registers.r[0] = pVM->pCallStack[pVM->CurrentStackTop].LocalMemory.MemorySize;

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
#ifdef WIN32
	OutputDebugStringA((char*)pTemp);
#endif
	free(pTemp);

	return VM_OK;
}

uint32_t SysGetTimer(struct VirtualMachine* pVM)
{
	uint64_t Timer;
#ifdef WIN32
	LARGE_INTEGER current, freq;
	QueryPerformanceCounter(&current);
	QueryPerformanceFrequency(&freq);

	Timer = (uint64_t)(1000000.0 * (double)(current.QuadPart - pVM->Timer.QuadPart) / (double)freq.QuadPart);

	pVM->Registers.r[0] = Timer & 0xffffffff;
	pVM->Registers.r[1] = (Timer >> 32);
#endif

	return VM_OK;
}

uint32_t SysUInt64Operations(struct VirtualMachine* pVM)
{
	uint32_t id = pVM->Registers.r[0];
	uint64_t op1 = *(uint64_t*)&pVM->Registers.r[1]; // r1-r2, need check for Big Endian
	uint64_t op2 = *(uint64_t*)&pVM->Registers.r[3]; // r3-r4

	switch(id){
		case VM_UINT64_ADD:
		{
			*(uint64_t*)&pVM->Registers.r[0] = op1 + op2;
		}
		break;
		case VM_UINT64_SUB:
		{
			*(uint64_t*)&pVM->Registers.r[0] = op1 - op2;
		}
		break;
		case VM_UINT64_MUL:
		{
			*(uint64_t*)&pVM->Registers.r[0] = op1 * op2;
		}
		break;
		case VM_UINT64_DIV:
		{
			if(op2 == 0){
				return VM_DIVIDE_BY_ZERO;
			}
			*(uint64_t*)&pVM->Registers.r[0] = op1 / op2;
		}
		break;
		case VM_UINT64_MOD:
		{
			if(op2 == 0){
				return VM_DIVIDE_BY_ZERO;
			}
			*(uint64_t*)&pVM->Registers.r[0] = op1 % op2;
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

// SetRender(id, enable)
// id - 0, Default Render
// id - 1, OpenGL ES 2.0
// enable - 0, disable
// enable - 1, enable
// return value
// 0 - success
// 1 - not available
// 2 - can't operate, because other render is working or no render is working

uint32_t SysSetRender(struct VirtualMachine* pVM)
{
	uint32_t id = pVM->Registers.r[0];
	uint32_t enable = pVM->Registers.r[1];

	if(id > 1 || enable > 1){
		return VM_INVALID_SYSCALL;
	}
	if(pVM->hDC == NULL){
		// no-render configuration
		pVM->Registers.r[0] = VM_RENDER_NOT_AVAILABLE;
		return VM_OK;
	}
#ifndef RENDER_ES20
	if(id == 1){
		pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
		return VM_OK;
	}
#else
//	if(id == 0){
//
//	}
#endif
	if(enable == 1){
		if(pVM->pRender != NULL)
		{
			// caller trying to enable render that is already running or while another running
			pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
			return VM_OK;
		}else{
			// probe to enable default render
			pVM->pRender = (struct Render*)malloc(sizeof(struct Render));
			memset(pVM->pRender, 0, sizeof(struct Render));
			
			if(RenderInit(pVM->pRender, pVM->hDC) != VM_OK){
				free(pVM->pRender);
				pVM->pRender = NULL;
				pVM->Registers.r[0] = VM_RENDER_NOT_AVAILABLE;
			}else{
				pVM->Registers.r[0] = VM_RENDER_OK;
			}
			return VM_OK;
		}
	}else{
		if(pVM->pRender == NULL)
		{
			// caller trying to disable render that is not running or while another running
			pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
			return VM_OK;
		}else{
			// probe to disable default render
			RenderDeInit(pVM->pRender, pVM->hDC);
			free(pVM->pRender);
			pVM->pRender = NULL;
			pVM->Registers.r[0] = VM_RENDER_OK;
			return VM_OK;
		}
	}

	return VM_OK;
}

uint32_t SysRenderClear(struct VirtualMachine* pVM)
{
	uint32_t Color = pVM->Registers.r[0];

	if(pVM->pRender == NULL){
		pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
		return VM_OK;
	}

	RenderClear(pVM->pRender, Color & 0xff, (Color >> 8) & 0xff, (Color >> 16) & 0xff, (Color >> 24) & 0xff);

	return VM_OK;
}

uint32_t SysRenderSwapBuffers(struct VirtualMachine* pVM)
{
	if(pVM->pRender == NULL){
		pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
		return VM_OK;
	}
	RenderSwapBuffers(pVM->pRender, pVM->hDC);

	return VM_OK;
}

uint32_t SysRenderDrawQuad(struct VirtualMachine* pVM)
{
	uint32_t pQuad = pVM->Registers.r[0];

	if(pVM->pRender == NULL){
		pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
		return VM_OK;
	}
	if(((uint64_t)pQuad + sizeof(struct Quad)) > pVM->GlobalMemorySize){
		return VM_DATA_ACCESS_VIOLATION;
	}
	RenderDrawQuad(pVM->pRender, (struct Quad*)(pVM->pGlobalMemory + pQuad));

	return VM_OK;
}