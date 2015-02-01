
#ifdef _WIN32
#define SYSCALL __fastcall
#endif

// using to return in user context
enum SysCallStatusCodes{
	VM_SYSCALL_OK,
	VM_SYSCALL_BLOCKED_BY_THREADS,
};

enum FloatOps{
	VM_FLOAT_MOD,
	VM_FLOAT_POW,
	VM_FLOAT_FRC,
	VM_FLOAT_COS,
	VM_FLOAT_SIN,
	VM_FLOAT_TAN,
};

enum Integer64Ops{
	VM_INT64_ADD,
	VM_INT64_SUB,
	VM_INT64_MUL,
	VM_SINT64_DIV,
	VM_UINT64_DIV,
	VM_SINT64_MOD,
	VM_UINT64_MOD,
	VM_INT64_SHL,
	VM_SINT64_SHR,
	VM_UINT64_SHR,
	VM_INT64_NEG,
};

enum DoubleOps{
	VM_DOUBLE_ADD,
	VM_DOUBLE_SUB,
	VM_DOUBLE_MUL,
	VM_DOUBLE_DIV,
	VM_DOUBLE_DTOF,
	VM_DOUBLE_FTOD,
};



//uint32_t SysCall(struct VirtualMachine* pVM, uint32_t SysCallId);

uint32_t SYSCALL SysSetGlobalMemory(struct VirtualMachine* pVM);
uint32_t SYSCALL SysRegisterCallback(struct VirtualMachine* pVM);
uint32_t SYSCALL SysUnRegisterCallback(struct VirtualMachine* pVM);
uint32_t SYSCALL SysDispatchCallbacks(struct VirtualMachine* pVM);
uint32_t SYSCALL SysDebugOutput(struct VirtualMachine* pVM);
uint32_t SYSCALL SysGetTimer(struct VirtualMachine* pVM);
uint32_t SYSCALL SysSleep(struct VirtualMachine* pVM);
uint32_t SYSCALL SysFloatOperations(struct VirtualMachine* pVM);
uint32_t SYSCALL SysInteger64Operations(struct VirtualMachine* pVM);
uint32_t SYSCALL SysDoubleOperations(struct VirtualMachine* pVM);
uint32_t SYSCALL SysSetRender(struct VirtualMachine* pVM);
uint32_t SYSCALL SysRenderCreateTexture(struct VirtualMachine* pVM);
uint32_t SYSCALL SysRenderUpdateTexture(struct VirtualMachine* pVM);
uint32_t SYSCALL SysRenderClear(struct VirtualMachine* pVM);
uint32_t SYSCALL SysRenderSetTexture(struct VirtualMachine* pVM);
uint32_t SYSCALL SysRenderSwapBuffers(struct VirtualMachine* pVM);
uint32_t SYSCALL SysRenderDrawQuad(struct VirtualMachine* pVM);
uint32_t SYSCALL SysDecodeImage(struct VirtualMachine* pVM);
uint32_t SYSCALL SysFileManager(struct VirtualMachine* pVM);