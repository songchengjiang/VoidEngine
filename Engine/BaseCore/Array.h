#ifndef _VE_ARRAY_
#define _VE_ARRAY_
#include "Prerequisites.h"

template<typename TYPE>
class veArray
{
	USE_VE_PTR
public:
	veArray()
	: USE_VE_PTR_INIT
	, _size(0)
	, _end(0)
	, _buffer(nullptr){
	}
	veArray(unsigned int arySize) 
		: USE_VE_PTR_INIT
		, _size(arySize)
		, _end(_size)
		, _buffer(new TYPE[_size]){
	}

	veArray(const veArray<TYPE> &ary)
		: USE_VE_PTR_INIT
		, _size(ary._size)
		, _end(ary._end)
		, _buffer(new TYPE[_size]){
		memcpy(_buffer, ary._buffer, _size * sizeof(TYPE));
	}
	~veArray(){
		VE_SAFE_DELETE_ARRAY(_buffer);
	}

	void push_back(const TYPE &val){
		reallocSize();
		_buffer[_end++] = val;
	}

	const TYPE& back() const{
		veAssert(_end);
		return _buffer[_end - 1];
	}

	void pop_back(){
		veAssert(_end);
		--_end;
	}

	size_t erase(size_t iter){
		veAssert(iter < _end);
		for (size_t i = iter + 1; i < _end; ++i){
			_buffer[i - 1] = _buffer[i];
		}
		--_end;
	}

	bool empty() const{
		return _end == 0;
	}

	void clear(){
		_end = 0;
	}

	const TYPE* buffer() const{
		return _buffer;
	}

	TYPE* buffer() {
		return _buffer;
	}

	size_t size() const{
		return _end;
	}

	void resize(size_t sz){
		if (_size < sz){
			resizeImp(sz);
			_end = _size;
		}
	}

	inline TYPE operator [] (const size_t i) const{
		veAssert(i < _end);
		return _buffer[i];
	}

	inline TYPE& operator [] (const size_t i){
		veAssert(i < _end);
		return _buffer[i];
	}

	inline veArray<TYPE>& operator = (const veArray<TYPE> & rkArray) {
		_size = rkArray._size;
		_end = rkArray._end;
		VE_SAFE_DELETE_ARRAY(_buffer);
		_buffer = new TYPE[_size];
		memcpy(_buffer, rkArray._buffer, _size * sizeof(TYPE));
		return *this;
	}

private:

	void reallocSize(){
		if (_size <= _end){
			if (_size == 0){
				_size = 1000;
				_buffer = new TYPE[_size];
			}
			else{
				size_t newSize = _size * _size;
				resizeImp(newSize);
			}
		}
	}

	void resizeImp(size_t sz){
		TYPE* newBuf = new TYPE[sz];
		if (_buffer) {
			memcpy(newBuf, _buffer, _size * sizeof(TYPE));
			VE_SAFE_DELETE_ARRAY(_buffer);
		}
		_buffer = newBuf;
		_size = sz;
	}

private:

	size_t _size;
	size_t _end;
	TYPE *_buffer;
};

typedef veArray<veReal> veRealArray;
typedef veArray<unsigned int> veUintArray;

#endif
