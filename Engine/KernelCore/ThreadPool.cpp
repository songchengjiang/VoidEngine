#include "ThreadPool.h"

void veThreadPool::start()
{
	if (!_stop) return;
	_stop = false;
	_threadPool.assign(_maxThreads, nullptr);

	for (unsigned int i = 0; i < _maxThreads; ++i) {
		_threadPool[i] = new std::thread([this] {
				for (;;)
				{
					std::function<void()> task;
					AsyncTaskCallBack callback;
					{
						std::unique_lock<std::mutex> lock(this->_queueMutex);
						this->_condition.wait(lock,
							[this] { return this->_stop || !this->_tasks.empty(); });
						if (this->_stop && this->_tasks.empty())
							return;
						task = std::move(this->_tasks.front());
						callback = std::move(this->_taskCallBacks.front());
						this->_tasks.pop();
						this->_taskCallBacks.pop();
					}

					task();
					//Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, callback] { callback.callback(callback.callbackParam); });
				}
		});
	}
}

void veThreadPool::stop()
{
	if (_stop) return;
	_stop = true;
	{
		std::unique_lock<std::mutex> lock(_queueMutex);

		while (_tasks.size())
			_tasks.pop();
		while (_taskCallBacks.size())
			_taskCallBacks.pop();
	}
	_condition.notify_all();
	for (auto &thread : _threadPool) {
		thread->join();
	}

	for (auto &thread : _threadPool) {
		VE_SAFE_DELETE(thread);
	}
	_threadPool.clear();
}

veThreadPool::veThreadPool(unsigned int maxThreads)
	: _maxThreads(maxThreads)
	, _stop(true)
{
}

veThreadPool::~veThreadPool()
{
	stop();
}