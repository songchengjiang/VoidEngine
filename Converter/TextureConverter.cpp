#include "TextureConverter.h"

TextureConverter::TextureConverter()
{

}

TextureConverter::~TextureConverter()
{

}

void TextureConverter::convert(const std::string &filePath, const std::string &outFile)
{
	if (filePath.empty()) return;
	std::string oFilePath = outFile;
	auto texture = gli::load(filePath);
	if (oFilePath.empty()) {
		std::string::size_type e = filePath.find_last_of(".");
		oFilePath = filePath.substr(0, e) + std::string(".ktx");
	}
	gli::save_ktx(texture, oFilePath);
}
