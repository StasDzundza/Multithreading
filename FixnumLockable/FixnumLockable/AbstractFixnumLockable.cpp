#include "AbstractFixnumLockable.h"

#include <algorithm>
#include <thread>

namespace thread_sync {
	thread_local int AbstractFixnumLockable::thread_id = -1;

	AbstractFixnumLockable::AbstractFixnumLockable(){
		allocator = new allocator::IDAllocator(100);
	}

	AbstractFixnumLockable::AbstractFixnumLockable(allocator::IDAllocator* allocator) {
		this->allocator = allocator;
	}

	unsigned int AbstractFixnumLockable::get_id() {
		while (locker.test_and_set()) { std::this_thread::yield(); }
		if (thread_id <= 0) {
			thread_id = allocator->alloc(&thread_id);
		}
		locker.clear();
		return thread_id;
	}

	bool AbstractFixnumLockable::register_thread() {
		while (locker.test_and_set()) { std::this_thread::yield(); }
		thread_id = allocator->alloc(&thread_id);
		locker.clear();
		return (thread_id < 0) ? false : true;
	}

	void AbstractFixnumLockable::unregister_thread() {
		allocator->free(thread_id);
	}

	void AbstractFixnumLockable::reset(unsigned int new_max_id) {
		while (locker.test_and_set()) { std::this_thread::yield(); }
		allocator->reset(new_max_id);
		locker.clear();
	}

	void AbstractFixnumLockable::reset(unsigned int new_min_id, unsigned  int new_max_id) {
		while (locker.test_and_set()) { std::this_thread::yield(); }
		allocator->reset(new_min_id, new_max_id);
		locker.clear();
	}
	void AbstractFixnumLockable::lock(){
		get_id();
	}
	void AbstractFixnumLockable::unlock(){
		unregister_thread();
	}
	bool AbstractFixnumLockable::try_lock(){
		int prev_id = thread_id;
		int new_id = get_id();
		if (prev_id < 0 && new_id >= 0) {
			return true;
		}
		else {
			return false;
		}

	}
	AbstractFixnumLockable::~AbstractFixnumLockable(){
		delete allocator;
	}
}