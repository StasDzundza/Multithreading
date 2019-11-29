#pragma once
#include<iostream>
#include<thread>
#include<vector>
#include"TestFixnumLockable.h"
#include"UnregisteredThreadException.h"

using std::cout;
using std::endl;
using std::vector;
using std::thread;
using std::atomic;
using namespace thread_sync;

namespace tests {
	void registration_test() {
		const int MaxNumberOfThreads = 50;
		const int NumberOfThreads = 200;
		AbstractFixnumLockable* testLocker = new TestFixnumLockable(MaxNumberOfThreads);
		atomic<int>counter = 0;
		vector<thread>threads(NumberOfThreads);
		for (int i = 0; i < NumberOfThreads; i++) {
			threads[i] = thread([&]() {
				int result = testLocker->get_id();
				if (result < 0)
					counter++;
				});
		}
		for (int i = 0; i < NumberOfThreads; i++)
			threads[i].join();

		if (counter != NumberOfThreads-MaxNumberOfThreads)
			cout << "Registration Test failed" << endl;
		else
			cout << "Registration Test passed" << endl;
		delete testLocker;
	}

	void unregistration_test() {
		AbstractFixnumLockable* testLocker = new TestFixnumLockable(3);
		try {
			testLocker->unregister_thread();//there should be exception/ thread is not registered
			cout << "Unregistration Test failed" << endl;
			return;
		}
		catch (UnregisteredThreadException & ex) {}

		testLocker->register_thread();
		testLocker->unregister_thread();
		cout << "Unregistration Test passed" << endl;
		delete testLocker;
	}

	void reset_test() {
		AbstractFixnumLockable* testLocker = new TestFixnumLockable(1);

		testLocker->register_thread();
		if (testLocker->reset(2)) {
			cout << "Reset test failed" << endl;
			delete testLocker;
			return;
		}
		testLocker->unregister_thread();
		if (testLocker->reset(2)) {
			cout << "Reset test passed" << endl;
			delete testLocker;
			return;
		}
	}

	void lock_unlock_test() {
		AbstractFixnumLockable* testLocker = new TestFixnumLockable(3);
		try {
			testLocker->unlock();//there should be exception/ thread is not registered
			cout << "Lock_Unlock test failed" << endl;
			delete testLocker;
			return;
		}
		catch (UnregisteredThreadException & ex) {}

		testLocker->lock();
		testLocker->unlock();
		testLocker->unregister_thread();
		cout << "Lock_Unlock test passed" << endl;
		delete testLocker;
	}

	void try_lock_test() {
		AbstractFixnumLockable* testLocker = new TestFixnumLockable(3);
		bool locked = testLocker->try_lock();
		if (!locked) {
			cout << "Try_Lock test failed" << endl;
			delete testLocker;
			return;
		}

		locked = testLocker->try_lock();
		if (locked) {
			cout << "Try_Lock test failed" << endl;
			delete testLocker;
			return;
		}

		testLocker->unregister_thread();

		locked = testLocker->try_lock();
		if (locked)
			cout << "Try_Lock test passed" << endl;
		else {
			cout << "Try_Lock test failed" << endl;
		}
		delete testLocker;
	}
}