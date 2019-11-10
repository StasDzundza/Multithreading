#pragma once

class BasicLockable {
public:
	virtual void lock() = 0;
	virtual void unlock() = 0;
};