#pragma once
#include"AbstractFixnumLockable.h"

namespace thread_sync {
	class TestFixnumLockable : public AbstractFixnumLockable{
	public:
		TestFixnumLockable(int max_number_of_threads);
		void unlock(int thread_id)override;
		void lock(int thread_id)override;

	};
}
