#include <stdio.h>
#include <jpeglib.h>
#define PTW32_STATIC_LIB
#include <pthread.h>
#include "..\WebVM.h"
#include "SysCallMedia.h"
#include "SysCallFile.h"



void init_source(j_decompress_ptr cinfo)
{

}

void jpeg_sleep(j_decompress_ptr cinfo, struct DecodeStruct* pInfo)
{
#ifndef _WIN32
	struct timespec req_time;
#endif
	if(pInfo->sleep >= MAX_SLEEP_DURATION){
		// we're sleeping for too long without fresh data, clean all memory and exit thread
		jpeg_destroy_decompress(cinfo);
		pInfo->pUser->status_code = VM_DECODER_REACHED_MAX_SLEEP_TIME;
		// exit
		free(pInfo);
		pthread_exit(NULL);
	}
#ifdef _WIN32
	Sleep(SLEEP_DURATION);
#else
	req_time.tv_sec = 0;
	req_time.tv_nsec = SLEEP_DURATION * 1000000; // ms
	nanosleep(&req_time, NULL);
#endif
	pInfo->sleep = pInfo->sleep + SLEEP_DURATION;
}

boolean fill_input_buffer(j_decompress_ptr cinfo)
{
	struct DecodeStruct* pInfo = (struct DecodeStruct*)cinfo->client_data;
	// be aware this thing can change in another thread so use local copy
	uint32_t available_size = pInfo->pFile->available_size;

	if(available_size < pInfo->src_decoded || available_size > pInfo->src_size){
		// someone changed size to be less than it was or bigger than buffer
		(*cinfo->err->error_exit)((j_common_ptr)cinfo);
	}
	if((available_size - pInfo->src_decoded) == 0){
		// suspend
		return FALSE;
	}

	cinfo->src->bytes_in_buffer = available_size - pInfo->src_decoded;
	cinfo->src->next_input_byte = pInfo->pSrc + pInfo->src_decoded;
	pInfo->src_decoded = available_size;
	// reset sleep timer
	pInfo->sleep = 0;
	
	return TRUE;
}

void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	struct DecodeStruct* pInfo = (struct DecodeStruct*)cinfo->client_data;

	if(num_bytes > 0){
		// need to suspend
		while((uint32_t)num_bytes > cinfo->src->bytes_in_buffer){
			num_bytes = num_bytes - cinfo->src->bytes_in_buffer;
			fill_input_buffer(cinfo);
			// sleep
			jpeg_sleep(cinfo, pInfo);
		}
		cinfo->src->bytes_in_buffer = cinfo->src->bytes_in_buffer - num_bytes;
		cinfo->src->next_input_byte = cinfo->src->next_input_byte + num_bytes;
	}
}

void term_source(j_decompress_ptr cinfo)
{

}

void error_exit(j_common_ptr cinfo)
{
	struct DecodeStruct* pInfo = (struct DecodeStruct*)cinfo->client_data;
	// something went wrong, clean all memory and exit thread
	pInfo->pUser->status_code = VM_DECODER_DECODE_FAILED;
	(*cinfo->err->output_message)(cinfo);
	jpeg_destroy_decompress((j_decompress_ptr)cinfo);
	free(pInfo);
	pthread_exit(NULL);
}

void jpeg_memory_src(j_decompress_ptr cinfo, const JOCTET* buffer, uint32_t size)
{
	if(cinfo->src == NULL){
		cinfo->src = (struct jpeg_source_mgr*)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(jpeg_source_mgr));
	}

	cinfo->src->init_source = init_source;
	cinfo->src->fill_input_buffer = fill_input_buffer;
	cinfo->src->skip_input_data = skip_input_data;
	cinfo->src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
	cinfo->src->term_source = term_source;

	cinfo->src->next_input_byte = buffer;
	cinfo->src->bytes_in_buffer = size;
	((struct DecodeStruct*)cinfo->client_data)->src_decoded = cinfo->src->bytes_in_buffer;
}

void* GetJPEGInfo(void* pArg)
{
	struct DecodeStruct* pInfo = (struct DecodeStruct*)pArg;
	struct ImageInfo* pImageInfo = (struct ImageInfo*)pInfo->pUser;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	pInfo->pUser->status_code = VM_DECODER_DECODING;
	cinfo.client_data = pInfo;
	/* Step 1: allocate and initialize JPEG decompression object */
	cinfo.err = jpeg_std_error(&jerr);
	cinfo.err->error_exit = error_exit;
	jpeg_create_decompress(&cinfo);
	/* Step 2: specify data source (eg, a file) */
	jpeg_memory_src(&cinfo, pInfo->pSrc, pInfo->pFile->available_size);
	/* Step 3: read file parameters with jpeg_read_header() */
	while(jpeg_read_header(&cinfo, TRUE) == JPEG_SUSPENDED){
		// sleep, then check if we need skip and if we can do so, otherwise sleep until we can or until reach sleep limit
		jpeg_sleep(&cinfo, pInfo);
	}
	/* Step 4: set parameters for decompression */
	cinfo.out_color_space = JCS_EXT_RGBA;
	/* Step 5: Start decompressor */
/*	while(jpeg_start_decompress(&cinfo) == JPEG_SUSPENDED){
		// sleep, then check if we need skip and if we can do so, otherwise sleep until we can or until reach sleep limit
		jpeg_sleep(&cinfo, pInfo);
	}*/
	jpeg_calc_output_dimensions(&cinfo);

	pImageInfo->width = cinfo.output_width;
	pImageInfo->height = cinfo.output_height;
	pImageInfo->bytes_per_pixel = cinfo.out_color_components;
	pImageInfo->format = VM_IMAGE_JPEG;
	pImageInfo->pformat = VM_PIXEL_FMT_RGB; // need check for gray jpegs
	pImageInfo->row_size = cinfo.output_width * cinfo.output_components;
	
	jpeg_abort_decompress(&cinfo);
	
	/* Step 8: Release JPEG decompression object */
	jpeg_destroy_decompress(&cinfo);
	pInfo->pUser->status_code = VM_DECODER_OK;
	free(pInfo);	

	return NULL;
}

void* DecodeJPEG(void* pArg)
{
	struct DecodeStruct* pInfo = (struct DecodeStruct*)pArg;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW* buffer;
	int32_t row_stride;

	pInfo->pUser->status_code = VM_DECODER_DECODING;
	cinfo.client_data = pInfo;
	/* Step 1: allocate and initialize JPEG decompression object */
	cinfo.err = jpeg_std_error(&jerr);
	cinfo.err->error_exit = error_exit;
	jpeg_create_decompress(&cinfo);
	/* Step 2: specify data source (eg, a file) */
	jpeg_memory_src(&cinfo, pInfo->pSrc, pInfo->pFile->available_size);
	/* Step 3: read file parameters with jpeg_read_header() */
	while(jpeg_read_header(&cinfo, TRUE) == JPEG_SUSPENDED){
		// sleep, then check if we need skip and if we can do so, otherwise sleep until we can or until reach sleep limit
		jpeg_sleep(&cinfo, pInfo);
	}

	/* Step 4: set parameters for decompression */
	cinfo.out_color_space = JCS_EXT_RGBA;
	/* Step 5: Start decompressor */
	while(jpeg_start_decompress(&cinfo) == JPEG_SUSPENDED){
		// sleep until we can or until reach sleep limit
		jpeg_sleep(&cinfo, pInfo);
	}
	
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);
	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */
	while(cinfo.output_scanline < cinfo.output_height){
		while(jpeg_read_scanlines(&cinfo, buffer, 1) == 0){
			// sleep until we can or until reach sleep limit
			jpeg_sleep(&cinfo, pInfo);
		}
		memcpy(pInfo->pDst, buffer[0], row_stride);
		pInfo->pDst = pInfo->pDst + row_stride;
	}
	/* Step 7: Finish decompression */
	while(jpeg_finish_decompress(&cinfo) == FALSE){
		// sleep until we can or until reach sleep limit
		jpeg_sleep(&cinfo, pInfo);
	}
	/* Step 8: Release JPEG decompression object */
	jpeg_destroy_decompress(&cinfo);
	pInfo->pUser->status_code = VM_DECODER_OK;

	free(pInfo);

	return NULL;
}