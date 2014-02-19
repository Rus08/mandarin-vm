

struct SysCall{
	char* name;
};

struct SysCall SysCalls[] = {
	"SetGlobalMemory",
	"SetLocalMemory",
	"RegisterCallback",
	"UnRegisterCallback",
	"DispatchCallbacks",
};