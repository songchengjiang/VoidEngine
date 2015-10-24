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

std::string veFile::readFileToBuffer(const std::string &filePath)
{
	FILE* file = fopen(filePath.c_str(), "rb");
	if (!file) return std::string("null");
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	//rewind(file);
	fseek(file, 0, SEEK_SET);
	if (size == 0) return nullptr;

	char *buffer = new char[size + 1];
	size_t result = fread(buffer, sizeof(char), size, file);
	buffer[result] = '\0';
    //veLog(buffer);
	std::string buf;
	buf.assign(buffer, result);
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
