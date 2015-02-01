#include <stdint.h>
#include <setjmp.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <assert.h>

#ifndef RENDER_ES20
#include <GL/gl.h>
#include "Render/RenderDefault.h"
#endif

#define REGISTER_MAX 32
#define STACK_START_SIZE 16
#define MAX_ALLOWED_GLOBAL_MEMORY 32 * 1048576
#define MAX_ALLOWED_STACK_SIZE 128
#define SLEEP_DURATION 20
#define MAX_SLEEP_DURATION 5000
#define MAX_ALLOWED_THREADS 10

//#define STAT_COUNTERS

enum VM_STATUS_CODE{
	VM_OK,
	VM_COMPLETE,
	VM_DISPATCH,
// instructions
	VM_INVALID_REPEAT_MODIFIER,
	VM_INVALID_OPCODE,
	VM_INVALID_LAST_INSTRUCTION,
	VM_CODE_ACCESS_VIOLATION,
	VM_DATA_ACCESS_VIOLATION,
	VM_DIVIDE_BY_ZERO,
	VM_INVALID_SYSCALL,
// callbacks
	VM_CALLBACK_NOT_REGISTERED,
	VM_INVALID_CALLBACK,
	VM_NOTINTIME_CALLBACK_CALL,
	VM_STACK_IS_TOO_BIG,
	VM_DATA_SECTOR_IS_TOO_BIG,
	VM_NOT_ENOUGH_MEMORY,
	VM_TOO_MANY_THREADS,
// render related

};

enum VM_CALLBACKS{
	VM_ONKEYDOWN,
	VM_ONKEYUP,
};

struct _LocalMemory{
//	uint8_t* pMemory;
	uint32_t MemorySize;
};

struct Call{
	uint32_t regPC; // Program Counter
	uint32_t regFLAGS;
	struct _LocalMemory LocalMemory;
};

enum FLAGS{
//	Int16BitFlag = 1,
	ZeroFlag = 2,
	SignFlag = 4,
};

struct _Registers{
	uint32_t r[32];
	uint32_t PC; // Program Counter
	uint8_t  REQ; // Equal or not
	uint32_t FLAGS;
};

struct VirtualMachine{
//		code
	uint8_t* pCode;
	uint32_t CodeSize;
//	global memory
	uint8_t* pGlobalMemory;
	uint32_t GlobalMemorySize;
//  call stack
	struct Call* pCallStack;
	uint32_t CallStackSize;
	uint32_t CurrentStackTop;
// 
	uint32_t* pImport;
	uint32_t ImportSize;
	uint32_t* pExport;
	uint32_t ExportSize;
	struct _Registers Registers;
	bool DispatchFlag;
	jmp_buf CheckPassJmpBuf;
// threads
	uint8_t* pThreads;
	bool	 ThreadAlive[MAX_ALLOWED_THREADS];
	uint32_t ThreadsNum;
	uint32_t Callbacks[2];
	struct Render* pRender;
#ifdef _WIN32
	HDC hDC;
	LARGE_INTEGER Timer;
#else
	uint64_t Timer;
#endif
#ifdef STAT_COUNTERS
	uint64_t Count;
	uint64_t ExecTable[128];
	uint64_t RegistersHit[32];
#endif
};

uint32_t VMCreate(struct VirtualMachine* pVM, uint8_t* pCode, uint32_t CodeSize, uint8_t* pData, uint32_t DataSize,
				  uint32_t* pImport, uint32_t ImportSize, uint32_t* pExport, uint32_t ExportSize, HDC hDC);
uint32_t VMRun(struct VirtualMachine* pVM, uint32_t RunCount);
uint32_t VMDestroy(struct VirtualMachine* pVM);
uint32_t VMPrintInfo(struct VirtualMachine* pVM, char* file_name);

uint32_t VMOnKeyDown(struct VirtualMachine* pVM, uint32_t Key);
uint32_t VMOnKeyUp(struct VirtualMachine* pVM, uint32_t Key);





