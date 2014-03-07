#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "..\VMAsmLib\VMAsmLib.h"




int main(int argc, char* argv[])
{
	FILE* fp = 0;
	char* pCode = 0;
	char* pData = 0;
	int code_size = 0;
	int data_size = 0;
	

	if(argc != 4){
		printf("Use VMAsm file.in code.out data.out.\n");
		return 1;
	}
	
	if(AsmFileToMemory(argv[1], &pCode, &code_size, &pData, &data_size) != -1){
		// write code segment
		fp = fopen(argv[2], "wb");
		if(fp == 0){
			printf("Can't open file %s for writing.\n", argv[2]);
			return 1;
		}

		fwrite(pCode, 1, code_size, fp);
		fclose(fp);
		free(pCode);
		
		if(pData != NULL){
			fp = fopen(argv[3], "wb");
			if(fp == 0){
				printf("Can't open file %s for writing.\n", argv[3]);
				return 1;
			}
			fwrite(pData, 1, data_size, fp);
			fclose(fp);
			free(pData);
		}
	}

	return 0;
}