#pragma once

#include"Lockable.h"
#include<thread>

using std::thread;

class FixnumLockable : public Lockable {
public:
	virtual unsigned int get_id(const thread::id& id) = 0;
	virtual bool _register(const thread::id& id) = 0;
	virtual void unregister(const thread::id& id) = 0;
protected:
	const int NumberOfThreads = 5;
};