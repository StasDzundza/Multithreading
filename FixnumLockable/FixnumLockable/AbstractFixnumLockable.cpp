#include "AbstractFixnumLockable.h"

AbstractFixnumLockable::AbstractFixnumLockable(){
	free_id = new bool[NumberOfThreads];
	for (int i = 0; i < NumberOfThreads; i++)
		free_id[i] = 0;
}

unsigned int AbstractFixnumLockable::get_id(const thread::id& id){
	return threads_id[id];
}

bool AbstractFixnumLockable::_register(const thread::id& id){
	if (threads_id.size() < NumberOfThreads) {
		for (unsigned int i = 0; i < NumberOfThreads; i++) {
			if (free_id[i] == 0) {
				free_id[i] = 1;
				threads_id.emplace(id, i);
				return true;
			}
		}
		return false;
	}
	return false;
}

void AbstractFixnumLockable::unregister(const thread::id& id){
	unsigned int index = threads_id[id];
	free_id[index] = 0;
	threads_id.erase(id);
}
