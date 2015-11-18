#ifndef _VE_LOOP_QUEUE_
#define _VE_LOOP_QUEUE_
#include "Prerequisites.h"
#include <functional>

#define CAPACITY 10000
template<class T>
class veLoopQueue
{
public:

	typedef std::function<bool(const T &, const T &)> SortFunc;

	veLoopQueue()
		: _front(0)
		, _end(0){}
	~veLoopQueue(){}

	bool empty() const {
		return _front == _end;
	}

	const T& front() const {
		veAssert(_front != _end);
		return _queue[_front];
	}

	T& front() {
		veAssert(_front != _end);
		return _queue[_front];
	}

	const T& back() const {
		veAssert(_front != _end);
		return _queue[dec(_end)];
	}

	T& back() {
		veAssert(_front != _end);
		return _queue[dec(_end)];
	}

	unsigned int size() const {
		return (_end - _front + CAPACITY) % CAPACITY;
	}

	bool pop_front() {
		veAssert(_front != _end);
		_front = inc(_front);
		return true;
	}

	bool push_back(const T& val) {
		veAssert(_front != (_end + 1) % CAPACITY);
		_queue[_end] = val;
		_end = inc(_end);
		return true;
	}

	void sort(const SortFunc &cmpFunc){
		if (size() <= 1) return;
		for (size_t f = _front; f != _end;){
			for (size_t p = (f + 1) % CAPACITY; p != _end;){
				if (!cmpFunc(_queue[f], _queue[p])){
					T temp = _queue[f];
					_queue[f] = _queue[p];
					_queue[p] = temp;
				}
				p = inc(p);
			}
			f = inc(f);
		}
	}

	void quickSort(const SortFunc &cmpFunc) {
		if (size() <= 1) return;
		quickSort(_front, dec(_end), cmpFunc);
	}

private:

	size_t inc(size_t iter) {
		++iter;
		return iter % CAPACITY;
	}

	size_t dec(size_t iter) {
		return iter != 0 ? --iter : CAPACITY - 1;
	}

	size_t partition(size_t low, size_t high, const SortFunc &cmpFunc)
	{
		T privotKey = _queue[low];
		while (low != high) {
			while (low != high  && cmpFunc(privotKey, _queue[high])) high = dec(high);
			if (low != high)
				std::swap(_queue[low], _queue[high]);
			while (low != high  && cmpFunc(_queue[low], privotKey)) low = inc(low);
			if (low != high)
				std::swap(_queue[low], _queue[high]);
		}
		return low;
	}


	void quickSort(size_t low, size_t high, const SortFunc &cmpFunc) {
		if (low != high) {
			size_t privotLoc = partition(low, high, cmpFunc);
			if (low != privotLoc)
				quickSort(low, dec(privotLoc), cmpFunc);
			if (high != privotLoc)
				quickSort(inc(privotLoc), high, cmpFunc);
		}
	}

private:

	T _queue[CAPACITY];
	size_t _front;
	size_t _end;
};

#endif
