#include "File-win32.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32)
#include "FileReaderWriter.h"
#include <windows.h>
#include <shlwapi.h>  

static inline std::string convertPathFormatToUnixStyle(const std::string& path)
{
	std::string ret = path;
	int len = ret.length();
	for (int i = 0; i < len; ++i)
	{
		if (ret[i] == '\\')
		{
			ret[i] = '/';
		}
	}
	return ret;
}

static std::wstring stringUtf8ToWideChar(const std::string& strUtf8)
{
	std::wstring ret;
	if (!strUtf8.empty()) {
		int nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, nullptr, 0);
		if (nNum) {
			WCHAR* wideCharString = new WCHAR[nNum + 1];
			wideCharString[0] = 0;

			nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wideCharString, nNum + 1);

			ret = wideCharString;
			delete[] wideCharString;
		}
		else {
			char str[256];
			sprintf(str, "Wrong convert to WideChar code:0x%x", GetLastError());
			veLog(str);
		}
	}
	return ret;
}

veFileWin32::veFileWin32()
{
	wchar_t pUtf16ExePath[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, pUtf16ExePath, sizeof(pUtf16ExePath));
	wchar_t *pUtf16DirEnd = wcsrchr(pUtf16ExePath, L'\\');
	char utf8ExeDir[MAX_PATH] = { 0 };
	int nNum = WideCharToMultiByte(CP_UTF8, 0, pUtf16ExePath, pUtf16DirEnd - pUtf16ExePath + 1, utf8ExeDir, sizeof(utf8ExeDir), nullptr, nullptr);
	_defaultResourcesPath = convertPathFormatToUnixStyle(utf8ExeDir);
}

veFileWin32::~veFileWin32()
{

}

veFileData* veFileWin32::readFileToBuffer(const std::string &filePath)
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

bool veFileWin32::isAbsolutePath(const std::string &filePath)
{
	if ((filePath.length() > 2
		&& ((filePath[0] >= 'a' && filePath[0] <= 'z') || (filePath[0] >= 'A' && filePath[0] <= 'Z'))
		&& filePath[1] == ':') || (filePath[0] == '/' && filePath[1] == '/'))
	{
		return true;
	}
	return false;
}

bool veFileWin32::isFileExist(const std::string &filePath)
{
	if (0 == filePath.length())
		return false;

	std::string absPath = getFullFilePath(filePath);
	DWORD attr = GetFileAttributesW(stringUtf8ToWideChar(absPath).c_str());
	if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
		return false;
	return true;
}

#endif
