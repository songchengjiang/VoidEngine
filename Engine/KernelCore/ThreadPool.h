#ifndef _VE_THREAD_POOL_BOX_
#define _VE_THREAD_POOL_BOX_
#include "Prerequisites.h"
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class VE_EXPORT veThreadPool
{
	friend class veSceneManager;
public:
    typedef std::function<void(void*)> TaskCallBack;
	struct AsyncTaskCallBack
	{
		TaskCallBack          callback;
		void*                 callbackParam;
	};
    
	void start();
	void stop();

    template<class Func>
    inline void enqueue(Func&& f);
    
protected:

    veThreadPool(unsigned int maxThreads = 3);
    ~veThreadPool();
    
protected:
    
	std::queue< std::function<void()> >   _tasks;
	//std::queue<AsyncTaskCallBack>       _taskCallBacks;

	std::vector<std::thread *>          _threadPool;
	unsigned int                        _maxThreads;

	// synchronization
	std::mutex _queueMutex;
	std::condition_variable _condition;
	bool _stop;
};

template<class Func>
inline void veThreadPool::enqueue(Func&& f)
{
	if (_stop) return;
	{
		std::unique_lock<std::mutex> lock(_queueMutex);
		_tasks.emplace(f);
	}
	_condition.notify_one();
}
#endif
