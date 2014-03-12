
enum FileOps{
	VM_FILE_GET_HANDLE,
	VM_FILE_SET_FLAGS,
	VM_FILE_READ_FILE,
	VM_FILE_WRITE_FILE,
	VM_FILE_CLOSE_HANDLE,
};

enum FileFlags{
	VM_FILE_URL,


};

enum FileStatusCodes{
	VM_FILE_OK,
	VM_FILE_INVALID_FLAG,
};

struct UserFileStruct{
	uint32_t status_code;
	uint32_t pBuf;
	uint32_t buf_size;
	uint32_t available_size;
};

struct FileStruct{
	struct UserFileStruct* pUser;
	uint32_t handle;
	uint8_t* pBuf;
	uint32_t buf_size;
	uint32_t available_size;
};


uint32_t VMFileManagerGetHandle(void);
uint32_t VMFileManagerSetFlags(uint32_t handle, uint32_t flag_description, uint32_t flag_value);
uint32_t VMFileManagerReadFile(uint32_t handle, struct FileStruct* pInfo);
uint32_t VMFileManagerWriteFile(uint32_t handle, struct FileStruct* pInfo);
uint32_t VMFileManagerCloseHandle(uint32_t handle);