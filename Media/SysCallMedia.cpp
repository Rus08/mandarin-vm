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
#include "SysCallFile.h"


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
			struct UserFileStruct* pFile;
			struct ImageInfo* pImageInfo;
			uint32_t file_addr = pVM->Registers.r[1];
			uint32_t struct_addr = pVM->Registers.r[2];

			// check input structs
			if(((uint64_t)file_addr + sizeof(struct UserFileStruct)) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			if(((uint64_t)struct_addr + sizeof(struct ImageInfo)) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}

			// check buffers
			pFile = (struct UserFileStruct*)(pVM->pGlobalMemory + file_addr);
			if(((uint64_t)pFile->pBuf + pFile->buf_size) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			pImageInfo = (struct ImageInfo*)(pVM->pGlobalMemory + struct_addr);
			pVM->Registers.r[0] = GetImageInfo(pVM, pFile, pImageInfo);
		}
		break;
		case VM_DECODE_JPEG:
		{
			struct UserFileStruct* pFile;
			struct UserDecodeStruct* pUserInfo;
			uint32_t file_addr = pVM->Registers.r[1];
			uint32_t addr = pVM->Registers.r[2];

			// check input structs
			if(((uint64_t)file_addr + sizeof(UserFileStruct)) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			if(((uint64_t)addr + sizeof(UserDecodeStruct)) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			// check buffers
			pFile = (struct UserFileStruct*)(pVM->pGlobalMemory + file_addr);
			if(((uint64_t)pFile->pBuf + pFile->buf_size) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			pUserInfo = (struct UserDecodeStruct*)(pVM->pGlobalMemory + addr);
			if(((uint64_t)pUserInfo->pDst + pUserInfo->dst_size) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			struct DecodeStruct* pInfo = (struct DecodeStruct*)malloc(sizeof(DecodeStruct));
			memset(pInfo, 0, sizeof(struct DecodeStruct));
			pInfo->pFile = pFile;
			pInfo->pUser = pUserInfo;
			pInfo->pSrc = pVM->pGlobalMemory + pFile->pBuf;
			pInfo->pDst = pVM->pGlobalMemory + pUserInfo->pDst;
			pInfo->src_size = pFile->buf_size;
			pthread_create(&thread, NULL, DecodeJPEG, pInfo);
		}
		break;
		default:
			assert(false);
			return VM_INVALID_SYSCALL;		
	}

	return VM_OK;
}



uint32_t GetImageInfo(struct VirtualMachine* pVM, struct UserFileStruct* pFile, struct ImageInfo* pImageInfo)
{
	uint8_t JPEG_id[] = { 0xff, 0xd8 };

	if(pFile->available_size >=2 && memcmp(pVM->pGlobalMemory + pFile->pBuf, JPEG_id, sizeof(JPEG_id)) == 0){
		// image is jpeg format
		struct DecodeStruct* pInfo;
		pthread_t thread;

		pInfo = (struct DecodeStruct*)malloc(sizeof(DecodeStruct));
		memset(pInfo, 0, sizeof(struct DecodeStruct));
		pInfo->pFile = pFile;
		pInfo->pUser = (struct UserDecodeStruct*)pImageInfo;
		pInfo->pSrc = pVM->pGlobalMemory + pFile->pBuf;
		pInfo->src_size = pFile->buf_size;
		pthread_create(&thread, NULL, GetJPEGInfo, pInfo);

		return VM_DECODER_OK;
	}else{
	//	assert(false);
		return VM_DECODER_NOT_ENOUGH_DATA;
	}


	return VM_DECODER_OK;
}

