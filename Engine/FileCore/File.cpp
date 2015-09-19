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
	if (!fileReader) return nullptr;
	return fileReader->readFile(filePath);
}

bool veFile::writeFile(void *data, const std::string &filePath)
{
	std::string ext = getFileExt(filePath);
	auto fileReader = FileReaderWriterRegistrar::instance()->getRegContent(ext);
	return fileReader->writeFile(data, filePath);
}

std::string veFile::readFileToBuffer(const std::string &filePath)
{
	FILE* file = fopen(filePath.c_str(), "r");
	if (!file) return std::string("null");
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);
	if (size == 0) return nullptr;

	char *buffer = new char[size];
	size_t result = fread(buffer, sizeof(char), size, file);
	if (result != size) buffer[result] = '\0';
	std::string buf(buffer);
	fclose(file);
	delete[] buffer;

	return buf;
}

std::string veFile::getFullFilePath(const std::string &filePath)
{
	return filePath;
}

std::string veFile::getFileExt(const std::string &filePath)
{
	return filePath.substr(filePath.find_last_of(".") + 1);
}
