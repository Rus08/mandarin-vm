

uint32_t InitThreads(struct VirtualMachine* pVM);
uint32_t GetAliveThreadsNum(struct VirtualMachine* pVM);
uint32_t AddThread(struct VirtualMachine* pVM, void* (*ThreadFunc)(void*), void* Arg);
uint32_t ThreadExit(struct VirtualMachine* pVM, bool NormalExit, void* Arg);
uint32_t DeInitThreads(struct VirtualMachine* pVM);