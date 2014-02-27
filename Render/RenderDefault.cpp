#include <stdlib.h>
//#include "../WebVM.h"
#include <stdint.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "RenderDefault.h"

uint32_t RenderProbe(struct Render* pRender)
{


	return VM_RENDER_OK;
}

uint32_t RenderInit(struct Render* pRender)
{
	PIXELFORMATDESCRIPTOR pfd = { 0 };
	int pixelformat = 0;
	GLint ViewPort[4];

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
	glGetIntegerv(GL_VIEWPORT, ViewPort);
	pRender->ScreenWidth = ViewPort[2];
	pRender->ScreenHeight = ViewPort[3];

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, pRender->ScreenWidth, pRender->ScreenHeight, 0, -1.0, 0.0);

	return VM_RENDER_OK;
}

uint32_t RenderClear(struct Render* pRender, uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Alpha)
{
	glClearColor(Red / 255.0f, Green / 255.0f, Blue / 255.0f, Alpha / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	return VM_RENDER_OK;
}

uint32_t RenderDrawQuad(struct Render* pRender, struct Quad* pQuad)
{
	glBegin(GL_QUADS);
	glColor4ubv((uint8_t*)&pQuad->color);
	glVertex3f(pQuad->x, pQuad->y, 0.5f);
	glVertex3f(pQuad->x, pQuad->y + pQuad->height, 0.5f);
	glVertex3f(pQuad->x + pQuad->width, pQuad->y + pQuad->height, 0.5f);
	glVertex3f(pQuad->x + pQuad->width, pQuad->y, 0.5f);	

	glEnd();

	return VM_RENDER_OK;
}

uint32_t RenderSwapBuffers(struct Render* pRender)
{
	SwapBuffers(pRender->hDC);
	
	return VM_RENDER_OK;
}

uint32_t RenderDeInit(struct Render* pRender)
{
	wglMakeCurrent(pRender->hDC, NULL);
	wglDeleteContext(pRender->hRC);
	pRender->hRC = NULL;

	return VM_RENDER_OK;
}