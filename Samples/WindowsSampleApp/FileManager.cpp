#include <stdlib.h>
#define CURL_STATICLIB
#include <curl.h>
#define PTW32_STATIC_LIB
#include <pthread.h>
#include <WebVM.h>
#include <Media/SysCallFile.h>


uint32_t VMFileManagerInit()
{
	curl_global_init(CURL_GLOBAL_ALL);

	return VM_FILE_OK;
}

size_t WriteMemoryCallback(void* pData, size_t size, size_t nmemb, void* userp)
{
	struct FileStruct* pDst = (struct FileStruct*)userp;
	uint32_t dsize;

	if(size * nmemb > (pDst->buf_size - pDst->available_size)){
		dsize = pDst->buf_size - pDst->available_size;
	}else{
		dsize = size * nmemb;
	}
	memcpy(pDst->pBuf + pDst->available_size, pData, dsize);
	pDst->available_size = pDst->available_size + dsize;
	pDst->pUser->available_size = pDst->available_size;
	
	char temp[64] = "";
	sprintf(temp, "Downloaded: %d\n", pDst->available_size);
	OutputDebugStringA(temp);
	return dsize;
}

uint32_t VMFileManagerGetHandle(void)
{
	CURL* curl;

	curl = curl_easy_init();
	if(curl != NULL){
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	}

	return (uint32_t)curl;
}

uint32_t VMFileManagerSetFlags(uint32_t handle, uint32_t flag_description, uint32_t flag_value)
{
	switch(flag_description){
		case VM_FILE_URL:
		{
			return curl_easy_setopt((CURL*)handle, CURLOPT_URL, (char*)flag_value);
		}
		break;

		default:
		{
			assert(false);
			return 1;
		}
	};

	return VM_FILE_OK;
}

void* VMFileReadThreadFunc(void* Arg)
{
	struct FileStruct* pInfo = (struct FileStruct*)Arg;
	CURLcode SC;

	SC = curl_easy_perform((CURL*)pInfo->handle);

	free(pInfo);
	return NULL;
}

uint32_t VMFileManagerReadFile(uint32_t handle, struct FileStruct* pInfo)
{
	pthread_t thread;
	curl_easy_setopt((CURL*)handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt((CURL*)handle, CURLOPT_WRITEDATA, (void*)pInfo);
	//curl_easy_setopt((CURL*)handle, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)1000);
	//curl_easy_setopt((CURL*)handle, CURLOPT_BUFFERSIZE, 4000);
	
	pInfo->pUser->available_size = 0;
	pthread_create(&thread, 0, VMFileReadThreadFunc, (void*)pInfo);
	/*if(SC != CURLE_OK){
		return VM_OK;
	}*/


	return VM_FILE_OK;
}

uint32_t VMFileManagerWriteFile(uint32_t handle, struct FileStruct* pInfo)
{
	return VM_FILE_OK;
}

uint32_t VMFileManagerCloseHandle(uint32_t handle)
{
	curl_easy_cleanup((CURL*)handle);
	return VM_FILE_OK;
}

uint32_t VMFileManagerDeInit()
{
	curl_global_cleanup();

	return VM_OK;
}