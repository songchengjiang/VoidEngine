#ifndef _VE_PTR_
#define _VE_PTR_
#include "Prerequisites.h"

template <class T>
class VE_Ptr
{
public:

	VE_Ptr(T *ptr)
		: _ptr(ptr)
		, _inUse(new size_t(1)){
	}

	VE_Ptr(const VE_Ptr &copy): _ptr(copy._ptr), _inUse(copy._inUse){
		ref();
	}

	~VE_Ptr(){

	}

	bool valid(){
		return _ptr != nullptr;
	}

	VE_Ptr& operator=(const VE_Ptr &rhs){
		unRef();
		_ptr = rhs._ptr;
		_inUse = rhs._inUse;
		ref();
		return *this;
	}

	T* operator->(){
		return _ptr;
	}

	const T* operator->() const{
		return _ptr;
	}

	T& operator*(){
		return *_ptr;
	}

	const T& operator*() const{
		return *_ptr;
	}

	bool operator==(const VE_Ptr &rhs) const{
		return _ptr == rhs._ptr;
	}

	bool operator!=(const VE_Ptr &rhs) const{
		return _ptr != rhs._ptr;
	}

private:

	void ref(){
		++*_inUse;
	}

	void unRef(){
		if (--*_inUse == 0){
			VE_SAFE_DELETE(_ptr);
			VE_SAFE_DELETE(_inUse);
		}
	}

private:

	T *_ptr;
	size_t *_inUse;
};

#endif