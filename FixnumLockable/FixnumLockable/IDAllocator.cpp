#include "IDAllocator.h"
#include "UnregisteredThreadException.h"

#include <thread>

namespace thread_sync::allocator {
	IDAllocator::IDAllocator(unsigned int max_id) : min_id(-1), max_id(-1),max_number_of_threads(50),number_of_registered_threads(0){
		init_values(0, max_id);
	}

	IDAllocator::IDAllocator(unsigned int min_id, unsigned int max_id) : min_id(-1), max_id(-1), max_number_of_threads(50),
		number_of_registered_threads(0) {
		init_values(min_id, max_id);
	}

	int IDAllocator::alloc() {
		while (locker.test_and_set()) { std::this_thread::yield(); }
		if (number_of_registered_threads >= max_number_of_threads) {
			locker.clear();
			return -1;//allocation has been failed
		}
		else {
			for (int i = 0; i < length_of_range; i++) {
				if (id_bitset[i] == 0) {
					id_bitset[i] = 1;
					int allocated_thread_id = i + min_id;//do shift on min_id
					number_of_registered_threads++;
					locker.clear();
					return allocated_thread_id;
				}
			}
			locker.clear();
			return -1;//there are no empty slots for this thread
		}
	}

	bool IDAllocator::free(int thread_id) {
		while (locker.test_and_set()) { std::this_thread::yield(); }
		if (thread_id < min_id || thread_id > max_id || id_bitset[thread_id - min_id] == 0) {
			locker.clear();
			throw new UnregisteredThreadException(thread_id);
		}
		else {
			id_bitset[thread_id - min_id] = 0;
			number_of_registered_threads--;
			locker.clear();
			return true;
		}
	}

	bool IDAllocator::reset(unsigned int max_id) {
		while (locker.test_and_set()) { std::this_thread::yield(); }
		bool reseted = reset_values(0, max_id);
		locker.clear();
		return reseted;
	}

	bool IDAllocator::reset(unsigned int min_id, unsigned int max_id) {
		while (locker.test_and_set()) { std::this_thread::yield(); }
		bool reseted = reset_values(min_id, max_id);
		locker.clear();
		return reseted;
	}

	void IDAllocator::set_max_number_of_threads(int max_number_of_threads){
		this->max_number_of_threads = max_number_of_threads;
	}

	bool IDAllocator::reset_values(unsigned int min_id, unsigned int max_id) {
		if (number_of_registered_threads > 0) {
			this->min_id = min_id;
			this->max_id = max_id;
			id_bitset.reset();
			length_of_range = max_id + 1;
			return true;
		}
		else {
			return false;
		}
	}

	void IDAllocator::init_values(unsigned int min_id, unsigned int max_id) {
		this->min_id = min_id;
		this->max_id = max_id;
		length_of_range = max_id - min_id + 1;
	}

}