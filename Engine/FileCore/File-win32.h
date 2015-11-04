#ifndef _VE_FILE_WIN32_
#define _VE_FILE_WIN32_
#include "File.h"

#if (VE_PLATFORM == VE_PLATFORM_WIN32)
#pragma warning( disable : 4005 )
class VE_EXPORT veFileWin32 : public veFile
{
public:
	veFileWin32();
	~veFileWin32();

	virtual veFileData* readFileToBuffer(const std::string &filePath) override;

protected:

	virtual bool isAbsolutePath(const std::string &filePath) override;
	virtual bool isFileExist(const std::string &filePath) override;
};
#endif

#endif

