#ifndef _VE_FILE_ANDROID_
#define _VE_FILE_ANDROID_
#include "FileCore/File.h"
#include <android/asset_manager.h>

class VE_EXPORT veFileAndroid : public veFile
{
public:
    static AAssetManager *ASSET_MANAGER;
    veFileAndroid();
    virtual ~veFileAndroid();

    virtual veFileData* readFileToBuffer(const std::string &filePath) override;
	virtual bool isFileExist(const std::string &absFilePath) override;

protected:

    virtual bool isAbsolutePath(const std::string &filePath) override;

};

#endif //ANDROID_FILE_ANDROID_H
