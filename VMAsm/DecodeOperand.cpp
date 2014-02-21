#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <stdbool.h>
#include "VMAsm.h"
#include "Util.h"

extern struct String* pStrings;
extern struct Label* pLabels;

extern int StringsNum;
extern int LabelsNum;


int DecodeOperand(char* Op, int IntMaxSize, int* pLastopintflag, bool SysCall, int StringNum)
{
	int op = 0;
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
			while(*Op != 0){
				if('0' <= Op[0] && Op[0] <= '9'){
					op = (op << 4) + (Op[0] - '0');
				}else if('a' <= Op[0] && Op[0] <= 'f'){
					op = (op << 4) + (Op[0] - 'a' + 10);
				}else if('A' <= Op[0] && Op[0] <= 'F'){
					op = (op << 4) + (Op[0] - 'A' + 10);
				}else{
					break;
				}
				Op = Op + 1;
			}
		}else if(strlen(Op) >= 3 && Op[0] == '0' && Op[1] == 'b'){
			// bit
			Op = Op + 2;
			while(*Op != 0){
				if('0' <= Op[0] && Op[0] <= '1'){
					op = (op << 1) + (Op[0] - '0');
				}else{
					break;
				}
				Op = Op + 1;
			}
		}else{
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
			}else if(SysCall == false){
				op = GetLabelAddr(Op);
				if(op == -1){
					printf("Error. Label %s at line %d not found!\n", Op, StringNum);
					return -1;
				}
			}else{
				op = GetSysCallAddr(Op);
				if(op == -1){
					printf("Error. API function %s at line %d not found!\n", Op, StringNum);
					return -1;
				}
			}
			if(*pLastopintflag != 0){
				printf("Error. Invalid operand at line %d!\n", StringNum);
				return -1;
			}
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

	return op;
}