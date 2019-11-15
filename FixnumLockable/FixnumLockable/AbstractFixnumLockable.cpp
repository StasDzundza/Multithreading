#include "AbstractFixnumLockable.h"

#include <algorithm>
#include <thread>

namespace thread_sync {
	thread_local int AbstractFixnumLockable::thread_id = -1;

	AbstractFixnumLockable::AbstractFixnumLockable(allocator::IDAllocator* allocator) : lock(ATOMIC_FLAG_INIT) {
		this->allocator = allocator;
	}

	unsigned int AbstractFixnumLockable::get_id() {
		while (lock.test_and_set()) { std::this_thread::yield(); }
		if (thread_id <= 0) {
			thread_id = allocator->alloc(&thread_id);
		}
		lock.clear();
		return thread_id;
	}

	bool AbstractFixnumLockable::register_thread() {
		while (lock.test_and_set()) { std::this_thread::yield(); }
		thread_id = allocator->alloc(&thread_id);
		lock.clear();
		return (thread_id < 0) ? false : true;
	}

	void AbstractFixnumLockable::unregister_thread() {
		allocator->free(thread_id);
	}

	void AbstractFixnumLockable::reset(unsigned int new_max_id) {
		while (lock.test_and_set()) { std::this_thread::yield(); }
		allocator->reset(new_max_id);
		lock.clear();
	}

	void AbstractFixnumLockable::reset(unsigned int new_min_id, unsigned  int new_max_id) {
		while (lock.test_and_set()) { std::this_thread::yield(); }
		allocator->reset(new_min_id, new_max_id);
		lock.clear();
	}
}