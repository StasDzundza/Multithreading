#pragma once
#include"BasicLockable.h"
namespace thread_sync {
	class Lockable : public BasicLockable {
	public:
		virtual void try_lock() = 0;
	};
}
