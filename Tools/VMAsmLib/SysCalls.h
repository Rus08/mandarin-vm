

struct SysCall{
	char* name;
};

struct SysCall SysCalls[] = {
	"SetGlobalMemory",
	"RegisterCallback",
	"UnRegisterCallback",
	"DispatchCallbacks",
	"DebugOutput",
	"GetTimer",
	"Sleep",
	"FloatOperations",
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
	"FileManager",
};