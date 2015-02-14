enum DecoderOps{
	VM_DECODER_GET_INFO,
	VM_DECODE_JPEG,
	VM_DECODE_PNG,
	VM_DECODE_GIF,
};

enum DECODER_STATUS_CODE{
	VM_DECODER_OK,
	VM_DECODER_DECODE_FAILED,
	VM_DECODER_REACHED_MAX_THREADS,
	VM_DECODER_DECODING,
	VM_DECODER_REACHED_MAX_SLEEP_TIME,
	VM_DECODER_NOT_ENOUGH_DATA,
	VM_DECODER_CANT_CREATE_THREAD,
};

enum VM_DECODER_FORMATS{
	VM_DECODER_JPEG,
	VM_DECODER_PNG,
	VM_DECODER_GIF,
};

struct ImageInfo{
	uint32_t status_code;
	uint8_t format;
	uint16_t width;
	uint16_t height;
	uint8_t	pformat;
	uint8_t bytes_per_pixel; // bytes per pixel
	uint32_t row_size;
};

enum IMAGE_FORMATS{
	VM_IMAGE_JPEG,
	VM_IMAGE_PNG,
	VM_IMAGE_GIF,
};

struct UserDecodeStruct{
	uint32_t status_code;
	uint32_t pDst;
	uint32_t dst_size;
	uint32_t decoded;
};
struct DecodeStruct{
	struct VirtualMachine* pVM;
	struct UserFileStruct* pFile;
	struct UserDecodeStruct* pUser;
	uint8_t* pSrc;
	uint8_t* pDst;
	uint32_t src_size;
	uint32_t src_decoded;
	//uint32_t need_to_skip;
	uint32_t sleep;
};

uint32_t GetImageInfo(struct VirtualMachine* pVM, struct UserFileStruct* pFile, struct DecodeStruct* pInfo, struct ImageInfo* pImageInfo);
void* GetJPEGInfo(void* pInfo);
void* DecodeJPEG(void* pInfo);


uint32_t VMFontManagerInit();
uint32_t VMFontManagerDeInit();