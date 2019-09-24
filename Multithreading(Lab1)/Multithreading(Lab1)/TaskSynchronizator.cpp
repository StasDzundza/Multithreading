#include "TaskSynchronizator.h"
#include <numeric>
#include <functional>
#include <chrono>


TaskSynchronizator::TaskSynchronizator() {
	oneOfResultsIsZero = false;
	workIsFinished = false;
	operationType = TaskSynchronizator::OperationType::Mult;
	cancelationType = TaskSynchronizator::CancelationType::Esc_Key;
	numberOfFunctions = 0;
	x = 100;
}

void TaskSynchronizator::addFunction(const function<int(int)>& f){
	functions.push_back(std::move(f));
	++numberOfFunctions;
	functionResults.push_back(-1);
}

void TaskSynchronizator::setX(const int& x){
	this->x = x;
}

void TaskSynchronizator::attachFunctionsToThreads(){
	threads.resize(numberOfFunctions + 1);//+1 because we also should save 1 thread for cancelation
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
	this->operationType = type;
}

void TaskSynchronizator::setCancelationType(CancelationType type){
	this->cancelationType = type;
}

void TaskSynchronizator::clearSettings(){
	threads.clear();
	functions.clear();
	functionResults.clear();
}

void TaskSynchronizator::start(){

	attachFunctionsToThreads();

	if (cancelationType == TaskSynchronizator::CancelationType::Esc_Key) {
		threads[numberOfFunctions] = thread([this]() { checkKeyCancelation(); });
		threads[numberOfFunctions].detach();
	}
	else {

		threads[numberOfFunctions] = thread([this]() { showPrompt(5); });
		threads[numberOfFunctions].detach();
	}

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
	if (operationType == OperationType::Mult)
		calculateMult();
}

void TaskSynchronizator::checkKeyCancelation()
{
	char c = 65;
	do {
		c = _getch();
	} while (c != 27);
	workIsFinished = true;
	stopAllThreads();
	cv.notify_one();
}

void TaskSynchronizator::showPrompt(const int& interval)
{
	std::this_thread::sleep_for(std::chrono::seconds(interval));
	std::cout << "Enter option : \n"
		"1.Continue calculation.\n"
		"2.Continue without prompt.\n"
		"3.Cancel calculation. \n";
	char choice = 65;
	do {
		std::cin >> choice;
	} while (choice != '1' && choice != '2' && choice != '3');

	switch (choice)
	{
		case '1': {
			showPrompt(interval);
			break;
		}
		case '2': {
			break;
		}
		case '3': {
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
