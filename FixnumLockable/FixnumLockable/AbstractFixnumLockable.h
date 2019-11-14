#pragma once

#include"FixnumLockable.h"
#include"IDAllocator.h"

class AbstractFixnumLockable : public FixnumLockable {
public:
	AbstractFixnumLockable(IDAllocator* allocator);
	unsigned int get_id()override;
	bool _register()override;
	void unregister()override;
	void reset(int new_max_id)override;
	void reset(int new_min_id,int new_max_id)override;
private:
	IDAllocator* allocator;
	thread_local static int thread_id;
};