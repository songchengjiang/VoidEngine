#ifndef _VE_FILE_MAC_
#define _VE_FILE_MAC_
#include "File.h"

#if (VE_PLATFORM == VE_PLATFORM_MAC || VE_PLATFORM == VE_PLATFORM_IOS)
class VE_EXPORT veFileApple : public veFile
{
public:
	veFileApple();
	~veFileApple();

	virtual veFileData* readFileToBuffer(const std::string &filePath) override;
	virtual bool isFileExist(const std::string &absFilePath) override;

protected:

	virtual bool isAbsolutePath(const std::string &filePath) override;
    
protected:
    
    void *_bundle;
};
#endif

#endif

