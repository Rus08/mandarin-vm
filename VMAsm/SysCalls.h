

struct SysCall{
	char* name;
};

struct SysCall SysCalls[] = {
	"SetGlobalMemory",
	"SetLocalMemory",
	"RegisterCallback",
	"UnRegisterCallback",
	"DispatchCallbacks",
	"DebugOutput",
	"GetTimer",
	"Integer64Operations",
	"DoubleOperations",
	"SetRender",
	"RenderClear",
	"RenderSwapBuffers",
	"RenderDrawQuad",
};