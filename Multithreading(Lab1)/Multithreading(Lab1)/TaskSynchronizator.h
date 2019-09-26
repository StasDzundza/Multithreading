#pragma once
#include <iostream>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <conio.h>
#include <utility>

using std::function;
using std::vector;
using std::thread;
using std::mutex;
using std::condition_variable;
using std::pair;

class TaskSynchronizator {
public:
	typedef pair<function<int(int)>, const char*> function_and_name;

	enum class OperationType {Mult};
	enum class CancelationType { Esc_Key,Prompt };
	TaskSynchronizator();
	void addFunction(const function<int(int)>& f,const char*name);
	void setX(int x);
	void start();
	void setOperationType(OperationType type);
	void setCancelationType(CancelationType type);
	void clearSettings();
private:
	void clearInput();
	void calculateMult();
	void stopAllThreads();
	void attachFunctionsToThreads();
	void notifyAboutWork(int resultOfCalculating);
	void calculate();
	void checkKeyCancelation();
	void showPrompt(int interval);
	int numberOfFunctions;
	bool oneOfResultsIsZero;
	bool workIsFinished;
	bool inputIsWaiting;
	int x;
	OperationType operationType;
	CancelationType cancelationType;
	mutex mtx;
	condition_variable cv;
	vector<thread>threads;
	vector <function_and_name>functions;
	vector<int>functionResults;
};