#include "IDAllocator.h"
#include "UnregisteredThreadException.h"

IDAllocator::IDAllocator(unsigned int maxID){
	this->min_id = 0;
	this->max_id = max_id;
	length_of_range = max_id + 1;
	id_array = new bool[length_of_range];//id starts from zero to maxID
	thread_id = new int* [MaxNumberOfThreads] {};
}

IDAllocator::IDAllocator(unsigned int min_id, unsigned int max_id){
	this->min_id = min_id;
	this->max_id = max_id;
	length_of_range = max_id - min_id + 1;
	id_array = new bool[length_of_range];//id starts from minID to maxID
	thread_id = new int* [MaxNumberOfThreads] {};
}

int IDAllocator::alloc(int* thread_id){
	if (number_of_registered_threads >= MaxNumberOfThreads)
		return -1;//allocation has been failed
	else {
		for (int i = 0; i < length_of_range; i++) {
			if (id_array[i] == false) {
				id_array[i] = true;
				int allocated_thread_id = i + min_id;
				number_of_registered_threads++;
				this->thread_id[size_of_thread_id_array++] = thread_id;
				return allocated_thread_id;
			}
		}
		return -1;//there are no empty slots for this thread
	}

}

bool IDAllocator::free(int thread_id){
	if (thread_id < min_id || thread_id > max_id || id_array[thread_id - min_id] == false) {
		throw new UnregisteredThreadException(thread_id);
	}	
	else {
		for (int i = 0; i < size_of_thread_id_array; i++) {
			if (this->thread_id[i] && thread_id == *(this->thread_id[i])) {
				*(this->thread_id[i]) = -1;
				this->thread_id[i] = nullptr;
				size_of_thread_id_array--;
				break;
			}
		}
		id_array[thread_id - min_id] = false;
		number_of_registered_threads--;
		return true;
	}
}

void IDAllocator::reset(unsigned int maxID){
	this->min_id = 0;
	this->max_id = maxID;
	delete[]id_array;
	length_of_range = max_id + 1;
	id_array = new bool[length_of_range];//id starts from zero to maxID
	clear_thread_id_array();
}

void IDAllocator::reset(unsigned int minId, unsigned int maxID){
	this->min_id = min_id;
	this->max_id = max_id;
	delete[]id_array;
	length_of_range = max_id - min_id + 1;
	id_array = new bool[length_of_range];//id starts from zero to maxID
	clear_thread_id_array();
}

void IDAllocator::clear_thread_id_array(){
	for (int i = 0; i < size_of_thread_id_array; i++) {
		if (this->thread_id[i]) {
			*(this->thread_id[i]) = -1;
			this->thread_id[i] = nullptr;
			size_of_thread_id_array--;
		}
	}
}
