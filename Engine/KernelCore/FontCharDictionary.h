#ifndef _VE_FONT_CHAR_DICTIONARY_
#define _VE_FONT_CHAR_DICTIONARY_
#include "Prerequisites.h"
#include "Font.h"
#include <unordered_map>

class veFontCharDictionary
{
public:

	~veFontCharDictionary();

	static veFontCharDictionary* instance();
	veFont::FontCharList* getOrCreateFontCharList(const std::string &fontFile, int fontSize);

private:
	veFontCharDictionary();
private:

	FT_Library _ftLibrary;
	std::unordered_map<std::string, veFont::FontCharList> _fontsDictionary;
};

#endif