#include "Font.h"
#include "FileCore/File.h"
#include "FontCharDictionary.h"

veFont::veFont(const std::string &fontFile, int fontSize)
	: USE_VE_PTR_INIT
	, _fontFile(fontFile)
	, _fontSize(0)
	, _fontCharList(nullptr)
{
	setFontSize(fontSize);
}

veFont::~veFont()
{
}

void veFont::setFontSize(int fontSize)
{
	if (_fontSize == fontSize) return;
	_fontSize = fontSize;
	_fontCharList = veFontCharDictionary::instance()->getOrCreateFontCharList(_fontFile, _fontSize);
}

veFont::CharBitmap* veFont::getCharBitmap(char ch)
{
	veAssert(ch < 128);
	return &_fontCharList->charBitmapList[ch];
}
