#include "File.h"
#include "FileReaderWriter.h"
#if defined(_MSC_VER)
#include "File-win32.h"
#endif

veFile::veFile()
{

}

veFile::~veFile()
{

}

veFile* veFile::instance()
{
#if defined(_MSC_VER)
	static veFileWin32 fileReader;
#endif
	return &fileReader;
}

void* veFile::readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param)
{
	std::string ext = getFileExt(filePath);
	auto fileReader = FileReaderWriterRegistrar::instance()->getRegContent(ext);
	if (!fileReader) return nullptr;
	return fileReader->readFile(sm, filePath, name, param);
}

bool veFile::writeFile(veSceneManager *sm, void *data, const std::string &filePath)
{
	std::string ext = getFileExt(filePath);
	auto fileReader = FileReaderWriterRegistrar::instance()->getRegContent(ext);
	return fileReader->writeFile(sm, data, filePath);
}

bool veFile::isSupportFile(const std::string &filePath)
{
	std::string ext = getFileExt(filePath);
	auto fileReader = FileReaderWriterRegistrar::instance()->getRegContent(ext);
	return fileReader == nullptr ? false : true;
}

std::string veFile::getFileExt(const std::string &filePath)
{
	return filePath.substr(filePath.find_last_of(".") + 1);
}
