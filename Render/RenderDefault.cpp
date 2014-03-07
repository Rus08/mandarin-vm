#include <stdlib.h>
//#include "../WebVM.h"
#include <stdint.h>
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "RenderDefault.h"

uint32_t RenderProbe(struct Render* pRender)
{


	return VM_RENDER_OK;
}

uint32_t RenderInit(struct Render* pRender, HDC hDC)
{
	PIXELFORMATDESCRIPTOR pfd = { 0 };
	int pixelformat = 0;
	GLint ViewPort[4];

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;

	pixelformat = ChoosePixelFormat(hDC, &pfd);
	if(pixelformat == 0){
		assert(false);
		return VM_RENDER_INIT_FAILED;
	}
	if(SetPixelFormat(hDC, pixelformat, &pfd) != TRUE){
		assert(false);
		return VM_RENDER_INIT_FAILED;
	}
	pRender->hRC = wglCreateContext(hDC);
	if(pRender->hRC == NULL){
		assert(false);
		return VM_RENDER_INIT_FAILED;
	}
	if(wglMakeCurrent(hDC, pRender->hRC) != TRUE){
		wglDeleteContext(pRender->hRC);
		assert(false);
		return VM_RENDER_INIT_FAILED;
	}
	glGetIntegerv(GL_VIEWPORT, ViewPort);
	pRender->ScreenWidth = ViewPort[2];
	pRender->ScreenHeight = ViewPort[3];

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, pRender->ScreenWidth, pRender->ScreenHeight, 0, -1.0, 0.0);

	glMatrixMode(GL_TEXTURE);
	//glScalef(1.0f, -1.0f, 1.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	return VM_RENDER_OK;
}

uint32_t RenderCreateTexture(struct Render* pRender, struct Texture* pTexture)
{
	if(pTexture->width > 2048 || pTexture->height > 2048){
		assert(false);
		return VM_RENDER_TEXTURE_TOO_BIG;
	}
	glGenTextures(1, &pRender->Textures[pRender->TexCount]);
	glBindTexture(GL_TEXTURE_2D, pRender->Textures[pRender->TexCount]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	pTexture->texid = pRender->Textures[pRender->TexCount];
	pRender->TexCount = pRender->TexCount + 1;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pTexture->width, pTexture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	return VM_RENDER_OK;
}

uint32_t RenderUpdateTexture(struct Render* pRender, struct Texture* pTexture, uint32_t offset, uint32_t size, uint8_t* pData)
{
	if(pTexture->width > 2048 || pTexture->height > 2048){
		assert(false);
		return VM_RENDER_TEXTURE_TOO_BIG;
	}

	glBindTexture(GL_TEXTURE_2D, pTexture->texid);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pTexture->width, pTexture->height, GL_RGBA, GL_UNSIGNED_BYTE, pData);

	return VM_RENDER_OK;
}

uint32_t RenderClear(struct Render* pRender, struct Rect* pRect, uint32_t Color)
{
	uint8_t Red = Color & 0xff;
	uint8_t Green = (Color >> 8) & 0xff;
	uint8_t Blue = (Color >> 16) & 0xff;
	uint8_t Alpha = (Color >> 24) & 0xff;

	if(pRect != NULL){
		glScissor(pRect->x, pRect->y, pRect->width, pRect->height);
		glEnable(GL_SCISSOR_TEST);
	}

	glClearColor(Red / 255.0f, Green / 255.0f, Blue / 255.0f, Alpha / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if(pRect != NULL){
		glDisable(GL_SCISSOR_TEST);
	}

	return VM_RENDER_OK;
}

uint32_t RenderSetTexture(struct Render* pRender, struct Texture* pTexture)
{
	glBindTexture(GL_TEXTURE_2D, pTexture->texid);

	return VM_RENDER_OK;
}

uint32_t RenderDrawQuad(struct Render* pRender, struct Quad* pQuad)
{
	//glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glColor4ubv((uint8_t*)&pQuad->color);
	//glColor4ub(0xff, 0, 0, 0xff);
	glVertex3f(pQuad->x, pQuad->y, 0.5f);
	glTexCoord2f(0.0f, 0.0f);

	//glColor4ub(0, 0xff, 0, 0xff);
	glVertex3f(pQuad->x, pQuad->y + pQuad->height, 0.5f);
	glTexCoord2f(0.0f, 1.0f);

	//glColor4ub(0, 0, 0xff, 0xff);
	glVertex3f(pQuad->x + pQuad->width, pQuad->y + pQuad->height, 0.5f);
	glTexCoord2f(1.0f, 1.0f);

	//glColor4ub(0xff, 0xff, 0xff, 0xff);
	glVertex3f(pQuad->x + pQuad->width, pQuad->y, 0.5f);	
	glTexCoord2f(1.0f, 0.0f);

	glEnd();

	int bu = glGetError();

	return VM_RENDER_OK;
}

uint32_t RenderSwapBuffers(struct Render* pRender, HDC hDC)
{
	SwapBuffers(hDC);
	
	return VM_RENDER_OK;
}

uint32_t RenderDeInit(struct Render* pRender, HDC hDC)
{
	wglMakeCurrent(hDC, NULL);
	wglDeleteContext(pRender->hRC);
	pRender->hRC = NULL;

	return VM_RENDER_OK;
}