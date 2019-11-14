#include "AbstractFixnumLockable.h"
#include <algorithm>

thread_local int AbstractFixnumLockable::thread_id = -1;

AbstractFixnumLockable::AbstractFixnumLockable(IDAllocator* allocator){
	this->allocator = allocator;
}

unsigned int AbstractFixnumLockable::get_id(){
	if (thread_id > 0) {
		return thread_id;
	}
	else {
		thread_id = allocator->alloc();
		return thread_id;
	}
}

bool AbstractFixnumLockable::_register(){
	thread_id = allocator->alloc();
	return (thread_id < 0) ? false : true;
}

void AbstractFixnumLockable::unregister(){
	allocator->free(thread_id);
	thread_id = -1;
	int* a = &thread_id;
}

void AbstractFixnumLockable::reset(int new_max_id){
	//set new range or max to allocator
}

void AbstractFixnumLockable::reset(int new_min_id, int new_max_id)
{
}
