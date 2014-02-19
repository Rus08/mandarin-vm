

uint32_t SysCall(struct VirtualMachine* pVM, uint32_t SysCallId);

uint32_t SysSetGlobalMemory(struct VirtualMachine* pVM);
uint32_t SysSetLocalMemory(struct VirtualMachine* pVM);
uint32_t SysRegisterCallback(struct VirtualMachine* pVM);
uint32_t SysUnRegisterCallback(struct VirtualMachine* pVM);
uint32_t SysDispatchCallbacks(struct VirtualMachine* pVM);