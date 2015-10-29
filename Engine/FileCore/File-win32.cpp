#include "File-win32.h"
#include "FileReaderWriter.h"

veFileWin32::veFileWin32()
{

}

veFileWin32::~veFileWin32()
{

}

std::string veFileWin32::readFileToBuffer(const std::string &filePath)
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

std::string veFileWin32::getFullFilePath(const std::string &filePath)
{
	return filePath;
}