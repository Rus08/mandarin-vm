

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
	"Sleep",
	"Integer64Operations",
	"DoubleOperations",
	"SetRender",
	"RenderCreateTexture",
	"RenderUpdateTexture",
	"RenderClear",
	"RenderSetTexture",
	"RenderSwapBuffers",
	"RenderDrawQuad",
	"DecodeImage",
};