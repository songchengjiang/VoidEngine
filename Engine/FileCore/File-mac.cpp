#include "File-mac.h"
#include "FileReaderWriter.h"

veFileMac::veFileMac()
{
}

veFileMac::~veFileMac()
{

}

veFileData* veFileMac::readFileToBuffer(const std::string &filePath)
{
	std::string fullPath = getFullFilePath(filePath);
	if (!isFileExist(fullPath)) 
		return nullptr;
	auto fileIter = _fileDataCache.find(fullPath);
	if (fileIter != _fileDataCache.end())
		return fileIter->second;

	FILE* file = fopen(fullPath.c_str(), "rb");
	if (!file) 
		return nullptr;
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	//rewind(file);
	fseek(file, 0, SEEK_SET);
	if (size == 0) return nullptr;

	char *buffer = new char[size + 1];
	size_t result = fread(buffer, sizeof(char), size, file);
	buffer[result] = '\0';
    //veLog(buffer);
	fclose(file);
	auto fileData = new veFileData();
	fileData->buffer = buffer;
	fileData->size = size + 1;
	_fileDataCache[fullPath] = fileData;
	return fileData;
}

bool veFileMac::isAbsolutePath(const std::string &filePath)
{
    if (filePath[0] == '/')
        return true;
	return false;
}

bool veFileMac::isFileExist(const std::string &filePath)
{
	if (0 == filePath.length())
		return false;
	return true;
}
