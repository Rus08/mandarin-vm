#include <stdlib.h>
#include <string.h>
#include "..\WebVM.h"
#include "..\SysCall.h"
#include "SysCallFile.h"



/*
	0 - GetHandle
	1 - SetFlags
	2 - ReadFile
	3 - WriteFile
	4 - CloseHandle

*/

uint32_t SYSCALL SysFileManager(struct VirtualMachine* pVM)
{

	uint32_t id = pVM->Registers.r[0];

	switch(id){
		case VM_FILE_GET_HANDLE:
		{
			pVM->Registers.r[0] = VMFileManagerGetHandle();
		}
		break;
		case VM_FILE_SET_FLAGS:
		{
			uint32_t handle = pVM->Registers.r[1];
			uint32_t flag_descr = pVM->Registers.r[2];
			uint32_t flag_value = pVM->Registers.r[3];

			switch(flag_descr){
				case VM_FILE_URL:
				{
					// check 0 ended strings
					if(flag_value > pVM->GlobalMemorySize){
						assert(false);
						return VM_DATA_ACCESS_VIOLATION;
					}
					uint32_t length = strnlen((char*)(pVM->pGlobalMemory + flag_value), pVM->GlobalMemorySize - flag_value);
					if(length > (pVM->GlobalMemorySize - flag_value)){
						assert(false);
						return VM_DATA_ACCESS_VIOLATION;
					}
					if(length == (pVM->GlobalMemorySize - flag_value) && *(pVM->pGlobalMemory + pVM->GlobalMemorySize) != 0){
						assert(false);
						return VM_DATA_ACCESS_VIOLATION;
					}
					flag_value = (uint32_t)(pVM->pGlobalMemory + flag_value);
				}
				break;
				default:
				{
					assert(false);
					pVM->Registers.r[0] = VM_FILE_INVALID_FLAG;
					return VM_OK;
				}
			};

			pVM->Registers.r[0] = VMFileManagerSetFlags(handle, flag_descr, flag_value);
		}
		break;
		case VM_FILE_READ_FILE:
		{
			uint32_t handle = pVM->Registers.r[1];
			uint32_t file_struct_addr = pVM->Registers.r[2];
			struct FileStruct* pStruct;
			
			pStruct = (struct FileStruct*)malloc(sizeof(struct FileStruct));

			if(((uint64_t)file_struct_addr + sizeof(struct UserFileStruct)) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			pStruct->pUser = (struct UserFileStruct*)(pVM->pGlobalMemory + file_struct_addr);

			if(((uint64_t)pStruct->pUser->pBuf + pStruct->pUser->buf_size) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			pStruct->handle = handle;
			pStruct->pBuf = pVM->pGlobalMemory + pStruct->pUser->pBuf;
			pStruct->buf_size = pStruct->pUser->buf_size;
			pStruct->available_size = 0;

			pVM->Registers.r[0] = VMFileManagerReadFile(handle, pStruct);
		}
		break;
		case VM_FILE_WRITE_FILE:
		{
			uint32_t handle = pVM->Registers.r[1];
			uint32_t file_struct_addr = pVM->Registers.r[2];
			struct FileStruct* pStruct = (struct FileStruct*)(pVM->pGlobalMemory + file_struct_addr);
			
			if(((uint64_t)file_struct_addr + sizeof(struct FileStruct)) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}
			if(((uint64_t)pStruct->pBuf + pStruct->buf_size) > pVM->GlobalMemorySize){
				assert(false);
				return VM_DATA_ACCESS_VIOLATION;
			}

			pVM->Registers.r[0] = VMFileManagerWriteFile(handle, pStruct);
		}
		break;
		case VM_FILE_CLOSE_HANDLE:
		{
			uint32_t handle = pVM->Registers.r[1];

			pVM->Registers.r[0] = VMFileManagerCloseHandle(handle);
		}
		break;
		default:
			assert(false);
			return VM_INVALID_SYSCALL;
	};

	return VM_OK;
}