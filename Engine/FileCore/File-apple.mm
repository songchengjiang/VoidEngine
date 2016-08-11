#include "File-apple.h"
#if (VE_PLATFORM == VE_PLATFORM_MAC || VE_PLATFORM == VE_PLATFORM_IOS)
#include "FileReaderWriter.h"
#import <Foundation/Foundation.h>

static NSFileManager* s_fileManager = [NSFileManager defaultManager];

veFileApple::veFileApple()
{
    _bundle = (__bridge void *)[NSBundle mainBundle];
}

veFileApple::~veFileApple()
{

}

veFileData* veFileApple::readFileToBuffer(const std::string &filePath)
{
	std::string fullPath = getFullFilePath(filePath);
	if (!isFileExist(fullPath)) 
		return nullptr;
    
    auto fileIter = _fileDataCache.find(fullPath);
    if (fileIter != _fileDataCache.end())
        return fileIter->second;
    
    std::string bundlePath = fullPath;
    if (bundlePath[0] != '/') {
        std::string path;
        std::string file;
        size_t pos = bundlePath.find_last_of("/");
        if (pos != std::string::npos) {
            file = bundlePath.substr(pos+1);
            path = bundlePath.substr(0, pos+1);
        }else {
            file = bundlePath;
        }
        
        NSBundle *bundle = (__bridge NSBundle *)_bundle;
        NSString* fPath = [bundle pathForResource:[NSString stringWithUTF8String:file.c_str()]
                                              ofType:nil
                                         inDirectory:[NSString stringWithUTF8String:path.c_str()]];
        bundlePath = [fPath UTF8String];
    }
    
    
    NSData *data = [[NSData alloc]initWithContentsOfFile:[NSString stringWithUTF8String: bundlePath.c_str()]];
    NSUInteger length = [data length];
    auto fileData = new veFileData();
    fileData->buffer = new char[length + 1];
    fileData->buffer[length] = '\0';
    fileData->size = length + 1;
    [data getBytes:fileData->buffer length:length];
	_fileDataCache[fullPath] = fileData;
	return fileData;
}

bool veFileApple::isAbsolutePath(const std::string &filePath)
{
    if (filePath[0] == '/')
        return true;
	return false;
}

bool veFileApple::isFileExist(const std::string &absFilePath)
{
    if (absFilePath.empty()) {
        return false;
    }
    
    bool ret = false;
    if (absFilePath[0] != '/') {
        std::string path;
        std::string file;
        size_t pos = absFilePath.find_last_of("/");
        if (pos != std::string::npos) {
            file = absFilePath.substr(pos+1);
            path = absFilePath.substr(0, pos+1);
        }else {
            file = absFilePath;
        }
        
        NSBundle *bundle = (__bridge NSBundle *)_bundle;
        NSString* fullpath = [bundle pathForResource:[NSString stringWithUTF8String:file.c_str()]
                                                           ofType:nil
                                                      inDirectory:[NSString stringWithUTF8String:path.c_str()]];
        if (fullpath != nil) {
            ret = true;
        }
    }else{
        // Search path is an absolute path.
        if ([s_fileManager fileExistsAtPath:[NSString stringWithUTF8String:absFilePath.c_str()]]) {
            ret = true;
        }
    }
    
    return ret;
}
#endif