
enum VM_RENDER_STATUS_CODE{
	VM_RENDER_OK,
	VM_RENDER_NOT_AVAILABLE,
	VM_RENDER_WRONG_CALL,
	VM_RENDER_INIT_FAILED,
	VM_RENDER_TEXTURE_TOO_BIG,
};

struct Texture{
	uint16_t width, height;
	uint32_t /*GLuint*/	 texid;
	int32_t /*GLint*/	 format;
};

enum PIXEL_FORMATS{
	VM_PIXEL_FMT_RGB,
	VM_PIXEL_FMT_RGBA,
};

struct Render{
	void* /*HGLRC*/ hRC;
	uint16_t ScreenWidth;
	uint16_t ScreenHeight;
	uint32_t /*GLuint*/ Textures[128]; // TODO vector
	uint32_t TexCount;
};

struct Rect{
	uint16_t x, y;
	uint16_t width, height;
};

struct Quad{
	float x, y;
	float width, height;
	float u, v, u1, v1;
	uint32_t color;
};


uint32_t RenderProbe(struct Render* pRender);
uint32_t RenderInit(struct Render* pRender, void* hDC);
uint32_t RenderCreateTexture(struct Render* pRender, struct Texture* pTexture);
uint32_t RenderUpdateTexture(struct Render* pRender, struct Texture* pTexture, uint32_t offset, uint32_t size, uint8_t* pData);
uint32_t RenderClear(struct Render* pRender, struct Rect* pRect, uint32_t Color);

uint32_t RenderSetTexture(struct Render* pRender, struct Texture* pTexture);
uint32_t RenderDrawQuad(struct Render* pRender, struct Quad* pQuad);

uint32_t RenderSwapBuffers(struct Render* pRender, void* hDC);
uint32_t RenderDeInit(struct Render* pRender, void* hDC);