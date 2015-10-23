#include "FontCharDictionary.h"
#include <sstream>
#include "FileCore/File.h"

veFontCharDictionary::veFontCharDictionary()
{
	FT_Error error = FT_Init_FreeType(&_ftLibrary);
	if (error) {
		veLog("Initialize FreeType error.");
	}
}

veFontCharDictionary::~veFontCharDictionary()
{
	for (auto &font : _fontsDictionary) {
		for (auto &chars : font.second.charBitmapList) {
			VE_SAFE_DELETE_ARRAY(chars.bitmap.buffer);
		}
	}
	FT_Done_FreeType(_ftLibrary);
}

veFontCharDictionary* veFontCharDictionary::instance()
{
	static veFontCharDictionary fontDictionary;
	return &fontDictionary;
}

veFont::FontCharList* veFontCharDictionary::getOrCreateFontCharList(const std::string &fontFile, int fontSize)
{
	std::stringstream ss; ss << fontFile << "#" << fontSize;
	auto iter = _fontsDictionary.find(ss.str());
	if (iter != _fontsDictionary.end())
		return &iter->second;

	std::string fontData = veFile::instance()->readFileToBuffer(fontFile);
	FT_Face face;
	FT_Error error = FT_New_Memory_Face(_ftLibrary, (const FT_Byte*)fontData.c_str(), fontData.size(), 0, &face);
	if (error) {
		veLog("New FreeType face error.");
		return nullptr;
	}
	int dpi = 72;
	int fontSizePoints = 64 * fontSize * VE_DEVICE_PIXEL_RATIO;
	FT_Set_Char_Size(face, fontSizePoints, fontSizePoints, dpi, dpi);

	veFont::FontCharList &charList = _fontsDictionary[ss.str()];
	charList.face = face;

	for (unsigned int i = 0; i < 128; ++i) {
		FT_Load_Char(face, i, FT_LOAD_RENDER);
		veFont::CharBitmap charbitmap;
		charbitmap.bitmap = face->glyph->bitmap;
		charbitmap.bitmap.buffer = new unsigned char[face->glyph->bitmap.rows * face->glyph->bitmap.width];
		memcpy(charbitmap.bitmap.buffer, face->glyph->bitmap.buffer, face->glyph->bitmap.rows * face->glyph->bitmap.width * sizeof(unsigned char));
		charbitmap.bitmap_left = face->glyph->bitmap_left;
		charbitmap.bitmap_top = face->glyph->bitmap_top;
		charbitmap.advance = face->glyph->advance;
		charList.charBitmapList.push_back(charbitmap);
	}

	return &charList;
}
