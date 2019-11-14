#pragma once
#include <exception>
#include <string>

using std::exception;
using std::string;

class UnregisteredThreadException : public exception {
public:
	UnregisteredThreadException(int thread_id) {
		this->thread_id = thread_id;
	}

	const char* what() {
		string what_msg = "Thread with id " + std::to_string(thread_id) + " has been tried to unlock, but it is not registered";
		return what_msg.c_str();
	}

	int get_thread_id() {
		return thread_id;
	}

private:
	int thread_id;
};