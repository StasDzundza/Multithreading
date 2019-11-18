#pragma once
#include "LockableConcepts.h"
#include "IDAllocator.h"

#include <atomic>

namespace thread_sync {
	class AbstractFixnumLockable : public FixnumLockable {
	public:
		AbstractFixnumLockable();
		AbstractFixnumLockable(allocator::IDAllocator* allocator);
		unsigned int get_id()override;
		bool register_thread()override;
		void unregister_thread()override;
		void reset(unsigned int new_max_id)override;
		void reset(unsigned int new_min_id, unsigned int new_max_id)override;
		void lock()override;
		void unlock()override;
		bool try_lock()override;
		virtual ~AbstractFixnumLockable();
	private:
		allocator::IDAllocator* allocator;
		thread_local static int thread_id;
		std::atomic_flag locker = ATOMIC_FLAG_INIT;
	};
}