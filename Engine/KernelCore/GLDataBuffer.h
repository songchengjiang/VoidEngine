#ifndef _VE_GL_DATA_BUFFER_
#define _VE_GL_DATA_BUFFER_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include <functional>
#include <unordered_map>

class VE_EXPORT veGLDataBuffer
{
    friend class veGLDataBufferManager;
public:
    typedef std::function<GLuint()> DataCreateFunc;
    typedef std::function<void(GLuint)> DataDestroyFunc;
    
    ~veGLDataBuffer();
    
    USE_VE_PTR;

    GLuint getData(unsigned int contextID) const;
    GLuint createData(unsigned int contextID);
    void destroyData(unsigned int contextID);
    void destroyAllData();
    
protected:
    
    veGLDataBuffer(const DataCreateFunc &cFunc, const DataDestroyFunc &dFunc);
    
protected:
    
    DataCreateFunc  _cFunc;
    DataDestroyFunc _dFunc;
    std::unordered_map<unsigned int, GLuint> _dataMap;
};

class VE_EXPORT veGLDataBufferManager
{
public:
    
    static veGLDataBufferManager* instance();
    
    veGLDataBuffer* createGLDataBuffer(const veGLDataBuffer::DataCreateFunc &cFunc, const veGLDataBuffer::DataDestroyFunc &dFunc);
    void destroyAllGLDataBuffer();
    
    
protected:
    
    std::vector< VE_Ptr<veGLDataBuffer> > _glDataBuffers;
};

#endif
