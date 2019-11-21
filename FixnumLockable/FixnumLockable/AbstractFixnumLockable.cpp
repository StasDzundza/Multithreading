#include "AbstractFixnumLockable.h"
#include "UnregisteredThreadException.h"

#include <thread>

namespace thread_sync {
	thread_local int AbstractFixnumLockable::thread_id = -1;

	AbstractFixnumLockable::AbstractFixnumLockable() :max_number_of_threads(50) {
		allocator = new allocator::IDAllocator(max_number_of_threads);
		allocator->set_max_number_of_threads(max_number_of_threads);
	}

	AbstractFixnumLockable::AbstractFixnumLockable(int max_number_of_threads){
		this->max_number_of_threads = max_number_of_threads;
		allocator = new allocator::IDAllocator(max_number_of_threads - 1);//because id starts from 0
		allocator->set_max_number_of_threads(max_number_of_threads);
	}

	AbstractFixnumLockable::AbstractFixnumLockable(allocator::IDAllocator* allocator) :max_number_of_threads(50) {
		this->allocator = allocator;
		allocator->set_max_number_of_threads(50);
	}

	int AbstractFixnumLockable::get_id(){
		if (thread_id < 0) {
			thread_id = allocator->alloc();
		}
		return thread_id;
	}

	bool AbstractFixnumLockable::register_thread() {
		thread_id = allocator->alloc();
		return (thread_id < 0) ? false : true;
	}

	void AbstractFixnumLockable::unregister_thread() {
		allocator->free(thread_id);
		thread_id = -1;
	}

	bool AbstractFixnumLockable::reset(unsigned int new_max_id) {
		return allocator->reset(new_max_id);
	}

	bool AbstractFixnumLockable::reset(unsigned int new_min_id, unsigned  int new_max_id) {
		return allocator->reset(new_min_id, new_max_id);
	}
	void AbstractFixnumLockable::lock(){
		lock(get_id());
	}
	void AbstractFixnumLockable::unlock(){
		if (thread_id < 0){
			throw new UnregisteredThreadException(thread_id);
		}
		unlock(thread_id);
		//unregister_thread();
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
	int AbstractFixnumLockable::get_max_number_of_threads(){
		return max_number_of_threads;
	}
	AbstractFixnumLockable::~AbstractFixnumLockable(){
		delete allocator;
	}
}