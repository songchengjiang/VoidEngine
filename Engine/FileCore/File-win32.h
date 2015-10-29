#ifndef _VE_FILE_WIN32_
#define _VE_FILE_WIN32_
#include "File.h"

class VE_EXPORT veFileWin32 : public veFile
{
public:
	veFileWin32();
	~veFileWin32();

	virtual std::string readFileToBuffer(const std::string &filePath) override;
	virtual std::string getFullFilePath(const std::string &filePath) override;

};

#endif

