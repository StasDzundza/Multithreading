#pragma once
#include<iostream>
#include<functional>
#include<vector>
#include<thread>
#include <mutex>
#include <condition_variable>
#include <concurrent_vector.h>

using std::function;
using std::vector;
using std::thread;
using std::mutex;
using std::condition_variable;

class TaskSynchronizator {
public:
	enum class OperationType {Mult};
	TaskSynchronizator();
	void addFunction(const function<int(int)>& f);
	void setX(int x);
	void start();
	void calculate();
	void attachFunctionsToThreads();
	void notifyAboutWork(int resultOfCalculating);
	void setOperationType(OperationType type);
private:
	void calculateMult();
	void stopAllThreads();
	int numberOfFunctions;
	bool oneOfResultsIsZero;
	bool workIsFinished;
	int x;
	OperationType type;
	mutex mtx;
	condition_variable cv;
	vector<thread>threads;
	vector <function<int(int)>>functions;
	vector<int>functionResults;
};