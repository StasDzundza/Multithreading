#pragma once
#include<iostream>
#include<functional>
#include<vector>
#include<thread>
#include <mutex>
#include <condition_variable>
#include <conio.h>

using std::function;
using std::vector;
using std::thread;
using std::mutex;
using std::condition_variable;

class TaskSynchronizator {
public:
	enum class OperationType {Mult};
	enum class CancelationType { Esc_Key,Prompt };
	TaskSynchronizator();
	void addFunction(const function<int(int)>& f);
	void setX(const int& x);
	void start();
	void setOperationType(OperationType type);
	void setCancelationType(CancelationType type);
	void clearSettings();
private:
	void calculateMult();
	void stopAllThreads();
	void attachFunctionsToThreads();
	void notifyAboutWork(int resultOfCalculating);
	void calculate();
	void checkKeyCancelation();
	void showPrompt(const int& interval);
	int numberOfFunctions;
	bool oneOfResultsIsZero;
	bool workIsFinished;
	int x;
	OperationType operationType;
	CancelationType cancelationType;
	mutex mtx;
	condition_variable cv;
	vector<thread>threads;
	vector <function<int(int)>>functions;
	vector<int>functionResults;
};