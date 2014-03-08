#include <stdlib.h>
#include <string.h>
#include "..\WebVM.h"
#include "..\SysCall.h"

enum FileOps{
	VM_FILE_GET_HANDLE,
	VM_FILE_SET_FLAGS,
	VM_FILE_READ_FILE,
	VM_FILE_WRITE_FILE,
	VM_FILE_CLOSE_HANDLE,
};
struct FileStruct{
	uint8_t* pBuf;
	uint32_t buf_size;
	uint32_t available_size;
};

uint32_t VMFileManagerGetHandle(void);
uint32_t VMFileManagerSetFlags(uint32_t handle, uint32_t flag_description, uint32_t flag_value);
uint32_t VMFileManagerReadFile(uint32_t handle, struct FileStruct* pInfo);
uint32_t VMFileManagerWriteFile(uint32_t handle, struct FileStruct* pInfo);
uint32_t VMFileManagerCloseHandle(uint32_t handle);

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

			pVM->Registers.r[0] = VMFileManagerSetFlags(handle, flag_descr, flag_value);
		}
		break;
		case VM_FILE_READ_FILE:
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