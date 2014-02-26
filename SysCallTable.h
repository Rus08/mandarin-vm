struct SysCall{
	uint32_t (*pFunc)(struct VirtualMachine* pVM);
};

struct SysCall SysCallTable[] = {
	SysSetGlobalMemory,
	SysSetLocalMemory,
	SysRegisterCallback,
	SysUnRegisterCallback,
	SysDispatchCallbacks,
	SysDebugOutput,
	SysGetTimer,
	SysUInt64Operations,
	SysDoubleOperations,
	SysSetRender,
	SysRenderClear,
	SysRenderSwapBuffers,
};