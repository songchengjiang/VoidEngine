#include "File-android.h"

AAssetManager* veFileAndroid::ASSET_MANAGER = nullptr;

veFileAndroid::veFileAndroid() {
    _defaultResourcesPath = "assets/";
}

veFileAndroid::~veFileAndroid() {

}

veFileData* veFileAndroid::readFileToBuffer(const std::string &filePath) {
    if (!veFileAndroid::ASSET_MANAGER)
        return nullptr;
    std::string fullPath = getFullFilePath(filePath);
    if (!isFileExist(fullPath))
        return nullptr;
    auto fileIter = _fileDataCache.find(fullPath);
    if (fileIter != _fileDataCache.end())
        return fileIter->second;

    const char* str = fullPath.c_str() + strlen("assets/");
    AAsset* asset = AAssetManager_open(veFileAndroid::ASSET_MANAGER, str, AASSET_MODE_UNKNOWN);
    if (asset) {
        off_t fileSize = AAsset_getLength(asset);
        char *buffer = new char[fileSize + 1];
        buffer[fileSize] = '\0';
        int bytesread = AAsset_read(asset, (void*)buffer, fileSize);
        auto fileData = new veFileData();
        fileData->buffer = buffer;
        fileData->size = fileSize + 1;
        //veLog("filebuffer: %s", buffer);
        _fileDataCache[fullPath] = fileData;
        AAsset_close(asset);
        return fileData;
    }
    return nullptr;
}

bool veFileAndroid::isAbsolutePath(const std::string &filePath) {
    if (strncmp(filePath.c_str(), _defaultResourcesPath.c_str(), _defaultResourcesPath.size()) == 0)
        return true;
    return false;
}

bool veFileAndroid::isFileExist(const std::string &absFilePath) {
    const char* str = absFilePath.c_str();
    if (filePath.find(_defaultResourcesPath) == 0) str += strlen("assets/");
    if (veFileAndroid::ASSET_MANAGER) {
        AAsset* aa = AAssetManager_open(veFileAndroid::ASSET_MANAGER, str, AASSET_MODE_UNKNOWN);
        if (aa) {
            AAsset_close(aa);
            //veLog("FileExist in Assets: %s", str);
            return  true;
        }
    }

    return false;
}