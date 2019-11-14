#pragma once
namespace thread_sync::allocator {
	class IDAllocator {
	public:
		IDAllocator(unsigned int min_id);
		IDAllocator(unsigned int min_id, unsigned int max_id);
		int alloc(int* thread_id);
		bool free(int id);
		void reset(unsigned int max_id);
		void reset(unsigned int min_id, unsigned int max_id);
		~IDAllocator();
	private:
		void clear_thread_id_array();
		void reset_values(unsigned int min_id, unsigned int max_id);
		void init_values(unsigned int min_id, unsigned int max_id);
		unsigned int min_id, max_id;
		const int MaxNumberOfThreads;
		int number_of_registered_threads;
		int length_of_range;
		bool* id_array;
		int** thread_id;
		int size_of_thread_id_array;
	};
}