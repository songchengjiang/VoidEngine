#include "GLDataBuffer.h"

veGLDataBuffer::veGLDataBuffer(const DataCreateFunc &cFunc, const DataDestroyFunc &dFunc)
    : USE_VE_PTR_INIT
    , _cFunc(cFunc)
    , _dFunc(dFunc)
{

}

veGLDataBuffer::~veGLDataBuffer()
{
}

GLuint veGLDataBuffer::getData(unsigned int contextID) const
{
    auto iter = _dataMap.find(contextID);
    if (iter != _dataMap.end())
        return iter->second;
    return 0;
}

GLuint veGLDataBuffer::createData(unsigned int contextID)
{
    auto iter = _dataMap.find(contextID);
    if (iter != _dataMap.end())
        return iter->second;
    GLuint glData = _cFunc();
    _dataMap[contextID] = glData;
    return glData;
}

void veGLDataBuffer::destroyData(unsigned int contextID)
{
    auto iter = _dataMap.find(contextID);
    if (iter != _dataMap.end()){
        _dFunc(iter->second);
        _dataMap.erase(iter);
    }
}

void veGLDataBuffer::destroyAllData()
{
    for (auto &data : _dataMap){
        _dFunc(data.second);
    }
    _dataMap.clear();
}

veGLDataBufferManager* veGLDataBufferManager::instance()
{
    static veGLDataBufferManager dfmgr;
    return &dfmgr;
}

veGLDataBuffer* veGLDataBufferManager::createGLDataBuffer(const veGLDataBuffer::DataCreateFunc &cFunc, const veGLDataBuffer::DataDestroyFunc &dFunc)
{
    auto glDataBuffer = new veGLDataBuffer(cFunc, dFunc);
    _glDataBuffers.push_back(glDataBuffer);
    return glDataBuffer;
}

void veGLDataBufferManager::destroyAllGLDataBuffer()
{
    for (auto &glDataBuf : _glDataBuffers){
        glDataBuf->destroyAllData();
    }
}