#ifndef _VE_DATA_POOL_
#define _VE_DATA_POOL_
#include "Prerequisites.h"
#include <list>

template<typename T>
class VE_EXPORT veDataPool
{
public:
    typedef typename std::list<T*> PoolList;
    typedef typename std::list<T*>::iterator PoolIterator;
    
    veDataPool(){};
    ~veDataPool(){};
    
    T* createData(){
        if (_locked.empty()) return nullptr;
        T* p = _locked.front();
        //_released.push_back(p);
        //_locked.erase(_locked.begin());
        _released.splice(_released.end(), _locked, _locked.begin());
        return p;
    };
    
    void lockLatestData(){
        _locked.push_back(*_releasedIter);
        _releasedIter = _released.erase(_releasedIter);
        if (_releasedIter != _released.begin() && _releasedIter != _released.end())
        {
            --_releasedIter;
        }
    };
    
    void lockData(T *data){
        PoolIterator tempIter = _releasedIter;
        T *ptr = getFirst();
        while (ptr)
        {
            if (ptr == data) break;
            ptr = getNext();
        }
        if (ptr)
            lockLatestData();
        _releasedIter = tempIter;
    }
    
    void lockAllDatas(){
        _locked.splice(_locked.end(), _released);
        //_locked.insert(_locked.end(), _released.begin(), _released.end());
        //_released.clear();
        _releasedIter = _released.begin();
    };
    
    T* getFirst(){
        _releasedIter = _released.begin();
        if (_releasedIter == _released.end()) return nullptr;
        return *_releasedIter;
    };
    
    T* getNext(){
        if (_releasedIter == _released.end()) return nullptr;
        ++_releasedIter;
        if (_releasedIter == _released.end()) return nullptr;
        return *_releasedIter;
    };
    
    const PoolList& getActiveDataList() const { return _released; };
    const PoolList& getUnActiveDataList() const { return _locked; };
    
    void addData(T* data){
        _locked.push_back(data);
    };
    
    bool empty() const { return _released.empty(); };
    
    void removeAllDatas(){
        lockAllDatas();
        for (auto iter : _locked){
            delete iter;
        }
        _locked.clear();
    };
    
private:
    
    PoolIterator _releasedIter;
    PoolList _released;
    PoolList _locked;
};

#endif
