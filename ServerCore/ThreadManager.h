#pragma once

#include <thread>
#include <functional>

/*------------------
	ThreadManager
-------------------*/

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void	Launch(function<void(void)> callback);
	void	Join();

private:
	static void InitTLS();
	static void DestroyTLS();

private:
	Mutex			_lock;
	vector<thread>	_threads;
};