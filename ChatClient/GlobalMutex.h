#pragma once

#include <mutex>

class global_mutex
{
private:
	std::mutex mtx;
	global_mutex() {}

public:

	static global_mutex* Instance()
	{
		static global_mutex instance;
		return &instance;
	}

	void Lock() { mtx.lock(); }
	void Unlock() { mtx.unlock(); }
	
	~global_mutex() {}
};