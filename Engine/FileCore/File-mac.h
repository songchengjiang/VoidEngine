#ifndef _VE_FILE_MAC_
#define _VE_FILE_MAC_
#include "File.h"

#if (VE_PLATFORM == VE_PLATFORM_MAC)
class VE_EXPORT veFileMac : public veFile
{
public:
	veFileMac();
	~veFileMac();

	virtual veFileData* readFileToBuffer(const std::string &filePath) override;
	virtual bool isFileExist(const std::string &filePath) override;

protected:

	virtual bool isAbsolutePath(const std::string &filePath) override;
};
#endif

#endif

