#include <stdlib.h>
#include "../WebVM.h"
#include <GL/gl.h>
#include "RenderDefault.h"

uint32_t RenderDefaultProbe(struct DefaultRender* pRender)
{


	return VM_OK;
}

uint32_t RenderDefaultInit(struct DefaultRender* pRender)
{
	PIXELFORMATDESCRIPTOR pfd = { 0 };
	int pixelformat = 0;

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;

	pixelformat = ChoosePixelFormat(pRender->hDC, &pfd);
	if(pixelformat == 0){
		return VM_RENDER_INIT_FAILED;
	}
	if(SetPixelFormat(pRender->hDC, pixelformat, &pfd) != TRUE){
		return VM_RENDER_INIT_FAILED;
	}
	pRender->hRC = wglCreateContext(pRender->hDC);
	if(pRender->hRC == NULL){
		return VM_RENDER_INIT_FAILED;
	}
	if(wglMakeCurrent(pRender->hDC, pRender->hRC) != TRUE){
		wglDeleteContext(pRender->hRC);
		return VM_RENDER_INIT_FAILED;
	}

	return VM_OK;
}

uint32_t RenderDefaultClear(struct DefaultRender* pRender, uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Alpha)
{
	glClearColor(Red / 255.0f, Green / 255.0f, Blue / 255.0f, Alpha / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	return VM_OK;
}

uint32_t RenderDefaultSwapBuffers(struct DefaultRender* pRender)
{
	SwapBuffers(pRender->hDC);
	
	return VM_OK;
}

uint32_t RenderDefaultDeInit(struct DefaultRender* pRender)
{
	wglMakeCurrent(pRender->hDC, NULL);
	wglDeleteContext(pRender->hRC);

	return VM_OK;
}