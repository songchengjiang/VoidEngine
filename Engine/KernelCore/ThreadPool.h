#ifndef _VE_THREAD_POOL_BOX_
#define _VE_THREAD_POOL_BOX_
#include "Prerequisites.h"
#include <functional>
#include <thread>
#include <mutex>
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
    inline void enqueue(const TaskCallBack& callback, void* callbackParam, Func&& f);
    
protected:

    veThreadPool(unsigned int maxThreads = 3);
    ~veThreadPool();
    
protected:
    
	std::queue< std::function<void()> > _tasks;
	//std::queue<AsyncTaskCallBack>       _taskCallBacks;

	std::vector<std::thread *>          _threadPool;
	unsigned int                        _maxThreads;

	// synchronization
	std::mutex _queueMutex;
	std::condition_variable _condition;
	bool _stop;
};

template<class Func>
inline void veThreadPool::enqueue(const TaskCallBack& callback, void* callbackParam, Func&& f)
{
	if (_stop) return;
	{
		std::unique_lock<std::mutex> lock(_queueMutex);
		//AsyncTaskCallBack taskCallBack;
		//taskCallBack.callback = callback;
		//taskCallBack.callbackParam = callbackParam;
		_tasks.emplace(f);
		//_taskCallBacks.emplace(taskCallBack);
	}
	_condition.notify_one();
}
#endif
