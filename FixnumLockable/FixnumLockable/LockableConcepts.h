#pragma once
namespace thread_sync {
	class BasicLockable {
	public:
		virtual void lock() = 0;
		virtual void unlock() = 0;
	};

	class Lockable : public BasicLockable {
	public:
		virtual void try_lock() = 0;
	};

	class FixnumLockable : public Lockable {
	public:
		virtual unsigned int get_id() = 0;
		virtual bool register_thread() = 0;
		virtual void unregister_thread() = 0;
		virtual void reset(unsigned int new_max_id) = 0;
		virtual void reset(unsigned int new_min_id, unsigned int new_max_id) = 0;
	};
}