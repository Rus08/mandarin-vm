#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <stdbool.h>
#include "VMAsm.h"
#include "Util.h"


int DecodeOperand(char* Op, unsigned int IntMaxSize, int* pLastopintflag, int StringNum, struct Segment* pSeg, unsigned int* pOut)
{
	unsigned int op = 0;
	int r = CheckForHiLo(Op);
	unsigned int mask = 0xffffffff;
	int shift = 0;

	if(r < 0){
		printf("Error. Invalid operand at line %d!\n", StringNum);
		return -1;
	}
	if(r == 1){
		Op[strlen(Op) - 1] = 0;
		Op = Op + DecodeHiLo(Op, &mask, &shift) + 1;
	}

	if(IsNumber(Op) == true){
		// operand is number
		if(strlen(Op) >= 3 && Op[0] == '0' && Op[1] == 'x'){
			// hex
			Op = Op + 2;
			op = DecodeHex(Op);
		}else if(strlen(Op) >= 3 && Op[0] == '0' && Op[1] == 'b'){
			// bit
			Op = Op + 2;
			op = DecodeBit(Op);
		}else{
			if(atoi(Op) < 0 && (unsigned int)(-atoi(Op) - 1) > IntMaxSize){
				printf("Error. Operand at line %d is too big!\n", StringNum);
				return -1;
			}
			op = atoi(Op);
		}
		/*if(*pLastopintflag != 0){
			printf("Error.Invalid operand at line %d!\n", StringNum);
			return -1;
		}*/
		*pLastopintflag = 1;
		op = (op & mask) >> shift;
		if(op > IntMaxSize){
			printf("Error. Operand at line %d is too big!\n", StringNum);
			return -1;
		}
	}else{
		if(Op[0] != 'r'){
			// operand is address label or char
			if(Op[0] == '\''){
				// operand is a char
				if(strlen(Op) != 3 || Op[2] != '\''){
					return -1;
				}
				op = (int)Op[1];
			}else{
				op = GetLabelAddr(Op, pSeg);
				if(op == -1){
					printf("Error. Label %s at line %d not found!\n", Op, StringNum);
					return -1;
				}
			}
			/*if(*pLastopintflag != 0){
				printf("Error. Invalid operand at line %d!\n", StringNum);
				return -1;
			}*/
			*pLastopintflag = 1;
		}else{
			// operand is register
			if(IsNumber(Op + 1) != true){
				// register number isn't number
				printf("Error. Invalid operand at line %d!\n", StringNum);
				return -1;
			}
			op = atoi(Op + 1);
			if(op > 31){
				// register number is too high
				printf("Error. Register number is too high at line %d!\n", StringNum);
				return -1;
			}
		}
	}
	*pOut = op;
	return 0;
}