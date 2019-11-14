#pragma once
#include <exception>
#include <string>

namespace thread_sync {
	class UnregisteredThreadException : public std::exception {
	public:
		UnregisteredThreadException(int thread_id) {
			this->thread_id = thread_id;
		}

		const char* what() {
			std::string what_msg = "Thread with id " + std::to_string(thread_id) + " has been tried to unlock, but it is not registered";
			return what_msg.c_str();
		}

		int get_thread_id() {
			return thread_id;
		}

	private:
		int thread_id;
	};
}