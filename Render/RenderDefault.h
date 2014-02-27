
enum VM_RENDER_STATUS_CODE{
	VM_RENDER_OK,
	VM_RENDER_NOT_AVAILABLE,
	VM_RENDER_WRONG_CALL,
	VM_RENDER_INIT_FAILED,
};

struct Render{
#ifdef WIN32
	HGLRC hRC;
#endif
	uint16_t ScreenWidth;
	uint16_t ScreenHeight;
};

struct Quad{
	float x, y;
	float width, height;
	float u, v, u1, v1;
	uint32_t color;
};


uint32_t RenderProbe(struct Render* pRender);
uint32_t RenderInit(struct Render* pRender, HDC hDC);
uint32_t RenderClear(struct Render* pRender, uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Alpha);

uint32_t RenderDrawQuad(struct Render* pRender, struct Quad* pQuad);

uint32_t RenderSwapBuffers(struct Render* pRender, HDC hDC);
uint32_t RenderDeInit(struct Render* pRender, HDC hDC);