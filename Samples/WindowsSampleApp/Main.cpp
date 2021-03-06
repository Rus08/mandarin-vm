#include <windows.h>
#include <stdio.h>
#include <WebVM.h>


HINSTANCE hInstance;
HWND hwndText;
HFONT hNewf1;
struct VirtualMachine VM;
HDC hDC;

//#ifdef _DEBUG
char* ErrorResolveTable[] = {
	"VM_OK",
	"VM_COMPLETE",
	"VM_DISPATCH",
// instructions
	"VM_INVALID_REPEAT_MODIFIER",
	"VM_INVALID_OPCODE",
	"VM_INVALID_LAST_INSTRUCTION",
	"VM_CODE_ACCESS_VIOLATION",
	"VM_DATA_ACCESS_VIOLATION",
	"VM_DIVIDE_BY_ZERO",
	"VM_INVALID_SYSCALL",
// callbacks
	"VM_CALLBACK_NOT_REGISTERED",
	"VM_INVALID_CALLBACK",
	"VM_NOTINTIME_CALLBACK_CALL",
	"VM_STACK_IS_TOO_BIG",
	"VM_DATA_SECTOR_IS_TOO_BIG",
	"VM_NOT_ENOUGH_MEMORY",
	"VM_TOO_MANY_THREADS",
};

char* GetErrorText(uint32_t id){
	return ErrorResolveTable[id];
}

//#endif

LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE:
        DestroyWindow (hwnd);
        break;

        case WM_DESTROY:
        PostQuitMessage (0);
        break;
		case WM_KEYDOWN:
		{
			VMOnKeyDown(&VM, wParam);
			switch(wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
			break;
			case VK_SPACE:
			{
				//SendMessage(hwnd, WM_SETTEXT, NULL, (LPARAM)"Hello world");
			}
			break;
			}
		}
		case WM_KEYUP:
		{
			VMOnKeyUp(&VM, wParam);
		}
		break;
        case WM_PAINT:
        {
           
        }
        break;
    }
    return DefWindowProc (hwnd, msg, wParam, lParam);
}

int WINAPI WinMain (HINSTANCE hInstanace, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX WindowClass;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.cbSize = sizeof (WNDCLASSEX);
    WindowClass.lpszClassName = "1";
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpfnWndProc = WndProc;
    WindowClass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    WindowClass.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    WindowClass.hCursor = LoadCursor (NULL, IDC_ARROW);
    WindowClass.style = 0;
    WindowClass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    RegisterClassEx (&WindowClass);

    HWND hwnd = CreateWindowEx (WS_EX_CLIENTEDGE,
                                "1",
                                "Printing Text in Win32 C/C++",
                                WS_OVERLAPPEDWINDOW,
                                315, 115,
                                1024, 800,
                                NULL,
                                NULL,
                                hInstance,
                                NULL);

    ShowWindow (hwnd, SW_SHOWNORMAL);

	//hwndText = CreateWindow("static", "ST_U", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, 0, 780, 300,
    //                          hwnd, (HMENU)(501), (HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE), NULL);

	// Create font for text windows
	//hNewf1 = CreateFont(18,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
	//	DEFAULT_PITCH|FF_DONTCARE,"Times New Roman Cyr");

	//SendMessage(hwndText, WM_SETFONT, (WPARAM)hNewf1, NULL);

	FILE* fp = 0;
	uint32_t file_size = 0, data_size = 0;
	uint8_t* pCode = 0, *pData = 0;
	uint32_t SC = 0;
	LARGE_INTEGER RunTime = { 0 }, start, end, freq;

	fp = fopen("test.bin", "rb");
	if(fp == 0){
		MessageBox(0, "File not found.", "Error", MB_OK);
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	pCode = (uint8_t*)malloc(file_size);

	fread(pCode, 1, file_size, fp);
	fclose(fp);

	fp = fopen("data.bin", "rb");
	if(fp != 0){
		fseek(fp, 0, SEEK_END);
		data_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		pData = (uint8_t*)malloc(data_size);

		fread(pData, 1, data_size, fp);
		fclose(fp);
	}

	// Create virtual machine
	hDC = GetDC(hwnd);
	SC = VMCreate(&VM, pCode, file_size, pData, data_size, NULL, 0, NULL, 0, hDC);
	
	if(SC != VM_OK){
		char temp[64] = "";
		sprintf(temp, "Error code: %s", GetErrorText(SC));
		MessageBox(0, temp, "Error", MB_OK);
		return 0;
	}

	MSG msg = { 0 };
    while(msg.message!=WM_QUIT)
	{
		QueryPerformanceCounter(&start);
		SC = VMRun(&VM, 50000);

		QueryPerformanceCounter(&end);
		RunTime.QuadPart = RunTime.QuadPart + (end.QuadPart - start.QuadPart);
		if(SC != VM_DISPATCH && SC != VM_OK){
			// error or vm complete
			break;
		}
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if(GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}else{
			// do something here
		}
	}
	QueryPerformanceFrequency(&freq);

	char temp[64] = "";
	if(SC != VM_COMPLETE){
		sprintf(temp, "Error code: %s", GetErrorText(SC));
		MessageBox(0, temp, "Error", MB_OK);
	}else{
		sprintf(temp, "Time of running: %.2f ms", 1000.0f * (float)RunTime.QuadPart / (float)freq.QuadPart);
		MessageBox(0, temp, "Info", MB_OK);
	}
	VMPrintInfo(&VM, "info.txt");
	VMDestroy(&VM);
	ReleaseDC(hwnd, hDC);


    return 0;
}