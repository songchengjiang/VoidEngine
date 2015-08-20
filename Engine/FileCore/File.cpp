#include "File.h"
#include "FileReaderWriter.h"

veFile::veFile()
{

}

veFile::~veFile()
{

}

veFile* veFile::instance()
{
	static veFile fileReader;
	return &fileReader;
}

void* veFile::readFile(const std::string &filePath)
{
	std::string ext = getFileExt(filePath);
	auto fileReader = FileReaderWriterRegistrar::instance()->getRegContent(ext);
	return fileReader->readFile(filePath);
}

bool veFile::writeFile(void *data, const std::string &filePath)
{
	std::string ext = getFileExt(filePath);
	auto fileReader = FileReaderWriterRegistrar::instance()->getRegContent(ext);
	return fileReader->writeFile(data, filePath);
}

std::string veFile::getFileExt(const std::string &filePath)
{
	return filePath.substr(filePath.find_last_of(".") + 1);
}
