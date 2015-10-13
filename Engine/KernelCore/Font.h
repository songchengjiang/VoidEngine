#ifndef _VE_FONT_
#define _VE_FONT_
#include "Prerequisites.h"
#include <ft2build.h>
#include FT_FREETYPE_H

class VE_EXPORT veFont
{
	friend class veText;
public:
	veFont(const std::string &fontFile, int fontSize = 16);
	~veFont();

	USE_VE_PTR;

	void setFontSize(int fontSize);
	int getFontSize() { return _fontSize; }

protected:

	FT_GlyphSlot getGlyphOfCharCode(unsigned long charCode);

protected:

	FT_Face _face;
	std::string _fontData;
	int _fontSize;
};

#endif