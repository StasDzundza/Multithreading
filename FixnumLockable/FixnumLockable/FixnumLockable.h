#pragma once

#include"Lockable.h"
namespace thread_sync {
	class FixnumLockable : public Lockable {
	public:
		virtual unsigned int get_id() = 0;
		virtual bool register_thread() = 0;
		virtual void unregister_thread() = 0;
		virtual void reset(unsigned int new_max_id) = 0;
		virtual void reset(unsigned int new_min_id, unsigned int new_max_id) = 0;
	};
}