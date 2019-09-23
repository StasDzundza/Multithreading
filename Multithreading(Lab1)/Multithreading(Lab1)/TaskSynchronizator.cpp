#include "TaskSynchronizator.h"
#include <numeric>
#include <functional>
#include <chrono>


TaskSynchronizator::TaskSynchronizator() : oneOfResultsIsZero{ false }, workIsFinished{ false }, type{ OperationType::Mult } {
	numberOfFunctions = 0;
	x = 100;
}

void TaskSynchronizator::addFunction(const function<int(int)>& f){
	functions.push_back(std::move(f));
	++numberOfFunctions;
	functionResults.push_back(-1);
}

void TaskSynchronizator::setX(int x){
	this->x = x;
}

void TaskSynchronizator::attachFunctionsToThreads(){
	threads.resize(numberOfFunctions);
	for (int i = 0; i < numberOfFunctions; i++)	{
		threads[i] = thread([this, i]() {
			int res = functions[i](this->x);
			functionResults[i] = res;
			notifyAboutWork(functionResults[i]);
			});
		threads[i].detach();
	}
}

void TaskSynchronizator::notifyAboutWork(int resultOfCalculating){
	if (workIsFinished) {
		stopAllThreads();
		return;
	}

	std::unique_lock<std::mutex> locker(mtx);

	std::cout << "Functions with id " << numberOfFunctions-- << " finished work with result " << resultOfCalculating << std::endl;

	if (resultOfCalculating == 0) {
		oneOfResultsIsZero = true;
		workIsFinished = true;
	}
	if (!numberOfFunctions || oneOfResultsIsZero || workIsFinished){
		workIsFinished = true;
		cv.notify_one();
	}
}

void TaskSynchronizator::setOperationType(OperationType type){
	this->type = type;
}

void TaskSynchronizator::start(){
	thread keyChecker(&TaskSynchronizator::checkKeyCancelation,this);
	keyChecker.detach();
	thread prompt(&TaskSynchronizator::showPrompt,this,10);
	prompt.detach();

	attachFunctionsToThreads();

	std::unique_lock<mutex>locker(mtx);
	cv.wait(locker, [this]() {return workIsFinished == true; });

	if (!oneOfResultsIsZero && !numberOfFunctions && workIsFinished){
		calculate();
	}
	else {
		if(oneOfResultsIsZero)
			std::cout << "Result of calculation is zero "<< std::endl;
		else {
			std::cout << "Calculation was canceled by user " << std::endl;
		}
	}
}

void TaskSynchronizator::calculate(){
	if (type == OperationType::Mult)
		calculateMult();
}

void TaskSynchronizator::checkKeyCancelation()
{
	char c = 65;
	do {
		if (_kbhit()) {
			c = _getch();
		}
	} while (c != 27);
	workIsFinished = true;
	stopAllThreads();
	cv.notify_one();
}

void TaskSynchronizator::showPrompt(int interval)
{
	std::this_thread::sleep_for(std::chrono::seconds(interval));
	std::cout << "Enter option : \n"
		"1.Continue calculation.\n"
		"2.Continue without prompt.\n"
		"3.Cancel calculation. \n";
	char choice = 65;
	do {
		if (_kbhit()){
			choice = _getch();
		}		
	} while (choice != '1' && choice != '2' && choice != '3');

	switch (choice)
	{
		case '1': {
			std::cout << "Continue was chosed.\n\n";
			showPrompt(interval);
			break;
		}
		case '2': {
			std::cout << "Continue without prompt was chosed.\n\n";
			break;
		}
		case '3': {
			std::cout << "Cancel was chosed.\n\n";
			stopAllThreads();
			workIsFinished = true;
			cv.notify_one();
			break;
		}
	}
}

void TaskSynchronizator::calculateMult(){
	int result = std::accumulate(functionResults.begin(), functionResults.end(), 1, std::multiplies<int>());
	std::cout << "Result of calculation is " << result << std::endl;
}

void TaskSynchronizator::stopAllThreads()
{
	for (int i = 0; i < threads.size(); i++) {
		threads[i].~thread();
	}
}
