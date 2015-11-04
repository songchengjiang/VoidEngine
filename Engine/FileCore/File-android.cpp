#include "File-android.h"

AAssetManager* veFileAndroid::ASSET_MANAGER = nullptr;

veFileAndroid::veFileAndroid() {
    _defaultResourcesPath = "assets/";
}

veFileAndroid::~veFileAndroid() {

}

veFileData* veFileAndroid::readFileToBuffer(const std::string &filePath) {

}

bool veFileAndroid::isAbsolutePath(const std::string &filePath) {
    if (strncmp(filePath.c_str(), _defaultResourcesPath.c_str(), _defaultResourcesPath.size()) == 0)
        return true;
    return false;
}

bool veFileAndroid::isFileExist(const std::string &filePath) {

}