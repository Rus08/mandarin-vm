#include <stdlib.h>
#include <string.h>
#include "..\WebVM.h"
#include "..\SysCall.h"
#include "..\Media\SysCallMedia.h"


// SetRender(id, enable)
// id - 0, Default Render
// id - 1, OpenGL ES 2.0
// enable - 0, disable
// enable - 1, enable
// return value
// 0 - success
// 1 - not available
// 2 - can't operate, because other render is working or no render is working

uint32_t SYSCALL SysSetRender(struct VirtualMachine* pVM)
{
	uint32_t id = pVM->Registers.r[0];
	uint32_t enable = pVM->Registers.r[1];

	if(id > 1 || enable > 1){
		assert(false);
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

uint32_t SYSCALL SysRenderCreateTexture(struct VirtualMachine* pVM)
{
	uint32_t addr = pVM->Registers.r[0];

	if(pVM->pRender == NULL){
		pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
		return VM_OK;
	}
	if(((uint64_t)addr + sizeof(struct Texture)) > pVM->GlobalMemorySize){
		assert(false);
		return VM_DATA_ACCESS_VIOLATION;
	}
	pVM->Registers.r[0] = RenderCreateTexture(pVM->pRender, (struct Texture*)(pVM->pGlobalMemory + addr));


	return VM_OK;
}

uint32_t SYSCALL SysRenderUpdateTexture(struct VirtualMachine* pVM)
{
	uint32_t tex_addr = pVM->Registers.r[0];
	uint32_t data_addr = pVM->Registers.r[1];
	uint32_t size = pVM->Registers.r[2];

	if(pVM->pRender == NULL){
		pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
		return VM_OK;
	}
	if(((uint64_t)tex_addr + sizeof(struct Texture)) > pVM->GlobalMemorySize){
		assert(false);
		return VM_DATA_ACCESS_VIOLATION;
	}
	if(((uint64_t)data_addr + size) > pVM->GlobalMemorySize){
		assert(false);
		return VM_DATA_ACCESS_VIOLATION;
	}
	pVM->Registers.r[0] = RenderUpdateTexture(pVM->pRender, (struct Texture*)(pVM->pGlobalMemory + tex_addr), 0, size, pVM->pGlobalMemory + data_addr);

	return VM_OK;
}

uint32_t SYSCALL SysRenderClear(struct VirtualMachine* pVM)
{
	uint32_t addr = pVM->Registers.r[0];
	uint32_t Color = pVM->Registers.r[1];	

	if(pVM->pRender == NULL){
		pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
		return VM_OK;
	}
	if((uint64_t)addr + sizeof(struct Rect) > pVM->GlobalMemorySize){
		assert(false);
		return VM_DATA_ACCESS_VIOLATION;
	}

	pVM->Registers.r[0] = RenderClear(pVM->pRender, (struct Rect*)(pVM->pGlobalMemory + addr), Color);

	return VM_OK;
}

uint32_t SYSCALL SysRenderSetTexture(struct VirtualMachine* pVM)
{
	uint32_t addr = pVM->Registers.r[0];

	if(pVM->pRender == NULL){
		pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
		return VM_OK;
	}
	if((uint64_t)addr + sizeof(struct Texture) > pVM->GlobalMemorySize){
		assert(false);
		return VM_DATA_ACCESS_VIOLATION;
	}
	pVM->Registers.r[0] = RenderSetTexture(pVM->pRender, (struct Texture*)(pVM->pGlobalMemory + addr));

	return VM_OK;
}

uint32_t SYSCALL SysRenderSwapBuffers(struct VirtualMachine* pVM)
{
	if(pVM->pRender == NULL){
		pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
		return VM_OK;
	}
	pVM->Registers.r[0] = RenderSwapBuffers(pVM->pRender, pVM->hDC);

	return VM_OK;
}

uint32_t SYSCALL SysRenderDrawQuad(struct VirtualMachine* pVM)
{
	uint32_t pQuad = pVM->Registers.r[0];

	if(pVM->pRender == NULL){
		pVM->Registers.r[0] = VM_RENDER_WRONG_CALL;
		return VM_OK;
	}
	if(((uint64_t)pQuad + sizeof(struct Quad)) > pVM->GlobalMemorySize){
		assert(false);
		return VM_DATA_ACCESS_VIOLATION;
	}
	pVM->Registers.r[0] = RenderDrawQuad(pVM->pRender, (struct Quad*)(pVM->pGlobalMemory + pQuad));

	return VM_OK;
}