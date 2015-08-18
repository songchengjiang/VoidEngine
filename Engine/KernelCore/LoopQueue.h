#ifndef _VE_LOOP_QUEUE_
#define _VE_LOOP_QUEUE_
#include "Prerequisites.h"
#include <functional>

#define CAPACITY 512
template<class T>
class veLoopQueue
{
public:

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

	const T& back() const {
		veAssert(_front != _end);
		return _queue[_end - 1];
	}

	unsigned int size() const {
		return (_end - _front + CAPACITY) % CAPACITY;
	}

	bool pop_front() {
		veAssert(_front != _end);
		inc(_front);
		return true;
	}

	bool push_back(const T& val) {
		veAssert(_front != (_end + 1) % CAPACITY);
		_queue[_end] = val;
		inc(_end);
		return true;
	}

	void sort(const std::function<bool(const T &, const T &)> &cmpFunc){
		for (size_t f = _front; f != _end;){
			for (size_t p = (f + 1) % CAPACITY; p != _end;){
				if (!cmpFunc(_queue[f], _queue[p])){
					T temp = _queue[f];
					_queue[f] = _queue[p];
					_queue[p] = temp;
				}
				inc(p);
			}
			inc(f);
		}
	}

private:

	void inc(size_t &iter) {
		++iter;
		iter = iter % CAPACITY;
	}

private:

	T _queue[CAPACITY];
	size_t _front;
	size_t _end;
};

#endif
