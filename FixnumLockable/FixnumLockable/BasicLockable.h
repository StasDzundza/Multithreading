#pragma once
namespace thread_sync {
	class BasicLockable {
	public:
		virtual void lock() = 0;
		virtual void unlock() = 0;
	};
}