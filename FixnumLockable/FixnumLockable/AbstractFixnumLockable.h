#pragma once

#include"FixnumLockable.h"
#include<unordered_map>//Hash Table

using std::unordered_map;

class AbstractFixnumLockable : public FixnumLockable {
public:
	AbstractFixnumLockable();
	unsigned int get_id(const thread::id& id)override;
	bool _register(const thread::id& id)override;
	void unregister(const thread::id& id)override;
private:
	unordered_map<thread::id, unsigned int> threads_id;
	bool* free_id;
};