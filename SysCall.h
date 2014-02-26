
enum UInt64Ops{
	VM_UINT64_ADD,
	VM_UINT64_SUB,
	VM_UINT64_MUL,
	VM_UINT64_DIV,
	VM_UINT64_MOD,
};

enum DoubleOps{
	VM_DOUBLE_ADD,
	VM_DOUBLE_SUB,
	VM_DOUBLE_MUL,
	VM_DOUBLE_DIV,
};

uint32_t SysCall(struct VirtualMachine* pVM, uint32_t SysCallId);

uint32_t SysSetGlobalMemory(struct VirtualMachine* pVM);
uint32_t SysSetLocalMemory(struct VirtualMachine* pVM);
uint32_t SysRegisterCallback(struct VirtualMachine* pVM);
uint32_t SysUnRegisterCallback(struct VirtualMachine* pVM);
uint32_t SysDispatchCallbacks(struct VirtualMachine* pVM);
uint32_t SysDebugOutput(struct VirtualMachine* pVM);
uint32_t SysGetTimer(struct VirtualMachine* pVM);
uint32_t SysUInt64Operations(struct VirtualMachine* pVM);
uint32_t SysDoubleOperations(struct VirtualMachine* pVM);
uint32_t SysSetRender(struct VirtualMachine* pVM);
uint32_t SysRenderClear(struct VirtualMachine* pVM);
uint32_t SysRenderSwapBuffers(struct VirtualMachine* pVM);