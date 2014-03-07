struct SysCall{
	uint32_t (SYSCALL *pFunc)(struct VirtualMachine* pVM);
};

struct SysCall SysCallTable[] = {
	SysSetGlobalMemory,
	SysSetLocalMemory,
	SysRegisterCallback,
	SysUnRegisterCallback,
	SysDispatchCallbacks,
	SysDebugOutput,
	SysGetTimer,
	SysSleep,
	SysInteger64Operations,
	SysDoubleOperations,
	SysSetRender,
	SysRenderCreateTexture,
	SysRenderUpdateTexture,
	SysRenderClear,
	SysRenderSetTexture,
	SysRenderSwapBuffers,
	SysRenderDrawQuad,
	SysDecodeImage,
};