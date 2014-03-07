#include <stdlib.h>
#include <string.h>
#define PTW32_STATIC_LIB
#include <pthread.h>
#ifndef _WIN32
#include <time.h>
#endif
#include "..\WebVM.h"
#include "..\SysCall.h"
#include "SysCallMedia.h"


/*
	uint32_t DecodeImage(uint32_t op_id, uint8_t* pSrc, uint32_t size, struct void* pStruct)
		
		op_id	- id of operation to perform from enum DecoderOps
		pSrc	- pointer to compressed file
		size    - size of available compressed file
		pStruct - pointer to ImageInfo or DecodeInfo struct depends on operation

		Return Value
		VM_DECODER_OK - success
		VM_DECODER_DECODE_FAILED - some error appears, maybe file corrupted or system haven't enough memory for decoder
		VM_DECODER_REACHED_MAX_THREADS - number of threads currently running by VM is reached their limit, wait a bit

*/
uint32_t SYSCALL SysDecodeImage(struct VirtualMachine* pVM)
{
	uint32_t id = pVM->Registers.r[0];
	pthread_t thread;

	switch(id){
		case VM_DECODER_GET_INFO:
		{
			uint32_t src_addr = pVM->Registers.r[1];
			uint32_t src_size = pVM->Registers.r[2];
			uint32_t struct_addr = pVM->Registers.r[3];

			if(((uint64_t)src_addr + src_size) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}

			if(((uint64_t)struct_addr + sizeof(struct ImageInfo)) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			pVM->Registers.r[0] = GetImageInfo(pVM->pGlobalMemory + src_addr, src_size, (struct ImageInfo*)(pVM->pGlobalMemory + struct_addr));
		}
		break;
		case VM_DECODE_JPEG:
		{
			struct DecodeStruct* pInfo;
			uint32_t addr = pVM->Registers.r[1];

			if(((uint64_t)addr + sizeof(UserDecodeStruct)) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}

			pInfo = (struct DecodeStruct*)malloc(sizeof(DecodeStruct));

			// check buffers
			pInfo->pUser = (struct UserDecodeStruct*)(pVM->pGlobalMemory + addr);
			if(((uint64_t)pInfo->pUser->pSrc + pInfo->pUser->size) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			if(((uint64_t)pInfo->pUser->pDst + pInfo->pUser->dst_size) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			pInfo->pSrc = pVM->pGlobalMemory + pInfo->pUser->pSrc;
			pInfo->pDst = pVM->pGlobalMemory + pInfo->pUser->pDst;
			pInfo->src_size = pInfo->pUser->size;
			pInfo->src_decoded = 0;
			pInfo->sleep = 0;
			pthread_create(&thread, NULL, DecodeJPEG, pInfo);
		}
		break;
		default:
			assert(false);
			return VM_INVALID_SYSCALL;		
	}

	return VM_OK;
}



uint32_t GetImageInfo(uint8_t* pSrc, uint32_t size, struct ImageInfo* pInfo)
{
	uint8_t JPEG_id[] = { 0xff, 0xd8 };

	if(size >=2 && memcmp(pSrc, JPEG_id, sizeof(JPEG_id)) == 0){
		// image is jpeg format
		return GetJPEGInfo(pSrc, size, pInfo);
	}else{
		assert(false);
		return VM_DECODER_DECODE_FAILED;
	}


	return VM_DECODER_OK;
}

