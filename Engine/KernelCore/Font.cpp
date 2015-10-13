#include "Font.h"
#include "FileCore/File.h"

class FTLibraryHandler
{
public:
	FTLibraryHandler(){
		FT_Error error = FT_Init_FreeType(&ft_library);
		if (error) {
			veLog("Initialize FreeType error.");
		}
	}
	~FTLibraryHandler() {
		FT_Done_FreeType(ft_library);
	}
	FT_Library ft_library;
};

veFont::veFont(const std::string &fontFile, int fontSize)
	: USE_VE_PTR_INIT
	, _fontSize(fontSize)
{
	static FTLibraryHandler ftlib;
	_fontData = veFile::instance()->readFileToBuffer(fontFile);
	//FT_Error error = FT_New_Face(ftlib.ft_library, fontFile.c_str(), 0, &_face);
	FT_Error error = FT_New_Memory_Face(ftlib.ft_library, (const FT_Byte*)_fontData.c_str(), _fontData.size(), 0, &_face);
	if (error) {
		veLog("New FreeType face error.");
	}

	setFontSize(fontSize);
}

veFont::~veFont()
{
}

void veFont::setFontSize(int fontSize)
{
	_fontSize = fontSize;
	int dpi = 72;
	int fontSizePoints = 64 * _fontSize * VE_DEVICE_PIXEL_RATIO;
	FT_Set_Char_Size(_face, fontSizePoints, fontSizePoints, dpi, dpi);
}

FT_GlyphSlot veFont::getGlyphOfCharCode(unsigned long charCode)
{
	//auto glyphIndex = FT_Get_Char_Index(_face, charCode);
	//FT_Load_Glyph(_face, glyphIndex, FT_LOAD_DEFAULT);
	//FT_Render_Glyph(_face->glyph, FT_RENDER_MODE_NORMAL);
	FT_Load_Char(_face, charCode, FT_LOAD_RENDER);
	return _face->glyph;
}
