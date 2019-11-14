#pragma once

#include"Lockable.h"
#include<thread>

using std::thread;

class FixnumLockable : public Lockable {
public:
	virtual unsigned int get_id() = 0;
	virtual bool _register() = 0;
	virtual void unregister() = 0;
	virtual void reset(int new_max_id) = 0;
	virtual void reset(int new_min_id,int new_max_id) = 0;
};