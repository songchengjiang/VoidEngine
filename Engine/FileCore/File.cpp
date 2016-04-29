#include "File.h"
#include "FileReaderWriter.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32)
#include "File-win32.h"
#elif (VE_PLATFORM == VE_PLATFORM_MAC)
#include "File-mac.h"
#elif (VE_PLATFORM == VE_PLATFORM_ANDROID)
#include "File-android.h"
#endif

veFile::veFile()
{

}

veFile::~veFile()
{
	for (auto &iter : _fileDataCache) {
		VE_SAFE_DELETE(iter.second);
	}
}

veFile* veFile::instance()
{
#if (VE_PLATFORM == VE_PLATFORM_WIN32)
	static veFileWin32 fileReader;
#elif (VE_PLATFORM == VE_PLATFORM_MAC)
    static veFileMac fileReader;
#elif (VE_PLATFORM == VE_PLATFORM_ANDROID)
	static veFileAndroid fileReader;
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

std::string veFile::getFullFilePath(const std::string &filePath)
{
	if (!isAbsolutePath(filePath)) {
		for (auto &searchPath : _searchPaths) {
			if (isFileExist(searchPath + filePath)) {
				return searchPath + filePath;
			}
		}
        if (isFileExist(_defaultResourcesPath + filePath)) {
            return _defaultResourcesPath + filePath;
        }

	}
	return filePath;
}

void veFile::addSearchPath(const std::string &path)
{
	std::string fullPath = path;
	if (!isAbsolutePath(path)) {
		fullPath = _defaultResourcesPath + path;
	}
	_searchPaths.push_back(fullPath);
}

void veFile::removeSearchPath(size_t idx)
{
	veAssert(idx < _searchPaths.size());
	_searchPaths.erase(_searchPaths.begin() + idx);
}

bool veFile::isSupportFile(const std::string &filePath)
{
	std::string ext = getFileExt(filePath);
	auto fileReader = FileReaderWriterRegistrar::instance()->getRegContent(ext);
	return fileReader == nullptr ? false : true;
}

std::string veFile::getFileExt(const std::string &filePath)
{
	std::string ext = filePath.substr(filePath.find_last_of(".") + 1);
	for (auto &c : ext) {
		c = tolower(c);
	}
	return ext;
}
