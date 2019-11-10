#pragma once
#include"BasicLockable.h"

class Lockable : public BasicLockable {
public:
	virtual void try_lock() = 0;
};
