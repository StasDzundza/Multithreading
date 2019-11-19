#pragma once

#include<atomic>
#include<bitset>

namespace thread_sync::allocator {
	class IDAllocator {
	public:
		IDAllocator(unsigned int max_id);
		IDAllocator(unsigned int min_id, unsigned int max_id);
		int alloc();
		bool free(int id);
		bool reset(unsigned int max_id);
		bool reset(unsigned int min_id, unsigned int max_id);
		void set_max_number_of_threads(int max_number_of_threads);
	private:
		std::atomic_flag locker = ATOMIC_FLAG_INIT;
		bool reset_values(unsigned int min_id, unsigned int max_id);
		void init_values(unsigned int min_id, unsigned int max_id);
		unsigned int min_id, max_id;
		int max_number_of_threads;
		int number_of_registered_threads;
		int length_of_range;
		std::bitset<1000> id_bitset;
	};
}