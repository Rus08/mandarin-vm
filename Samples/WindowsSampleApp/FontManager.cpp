#include <WebVM.h>
#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library font_lib;


uint32_t VMFontManagerInit()
{
	uint32_t RC = 0;

	RC = FT_Init_FreeType(&font_lib);
	if(RC != 0){
		assert(false);
		return RC;
	}

	return VM_OK;
}

uint32_t VMFontManagerDeInit()
{
	FT_Done_FreeType(font_lib);

	return VM_OK;
}