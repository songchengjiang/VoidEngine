#ifndef _VE_FONT_
#define _VE_FONT_
#include "Prerequisites.h"
#include <ft2build.h>
#include FT_FREETYPE_H

class VE_EXPORT veFont
{
	friend class veFontCharDictionary;
	friend class veText;
public:

	veFont(const std::string &fontFile, int fontSize = 16);
	~veFont();

	USE_VE_PTR;

	void setFontSize(int fontSize);
	int getFontSize() { return _fontSize; }

protected:

	struct CharBitmap
	{
		FT_Bitmap         bitmap;
		FT_Int            bitmap_left;
		FT_Int            bitmap_top;
		FT_Vector         advance;
	};

	struct FontCharList
	{
		FT_Face face;
		std::vector<CharBitmap> charBitmapList;
	};

	CharBitmap* getCharBitmap(char ch);

protected:

	FontCharList *_fontCharList;
	std::string _fontFile;
	int _fontSize;
};

#endif