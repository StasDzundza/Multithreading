#include "TaskSynchronizator.h"
#include <numeric>
#include <functional>
#include <chrono>
#include <Windows.h>


TaskSynchronizator::TaskSynchronizator() {
	oneOfResultsIsZero = false;
	workIsFinished = false;
	operationType = TaskSynchronizator::OperationType::Mult;
	cancelationType = TaskSynchronizator::CancelationType::Esc_Key;
	numberOfFunctions = 0;
	x = 0;
}

void TaskSynchronizator::addFunction(const function<int(int)>& f, const char* name) {
	functions.push_back(std::make_pair(std::move(f), name));
	++numberOfFunctions;
	functionResults.push_back(-123);
}

void TaskSynchronizator::setX(int x){
	this->x = x;
}

void TaskSynchronizator::attachFunctionsToThreads(){
	threads.resize(numberOfFunctions + 1);//+1 because we also should save 1 thread for cancelation
	for (int i = 0; i < numberOfFunctions; i++)	{
		threads[i] = thread([this, i]() {
			int res = functions[i].first(this->x);
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
	numberOfFunctions--;

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

	std::cout << "Calculation started" << std::endl;
	if (cancelationType == TaskSynchronizator::CancelationType::Esc_Key) {
		threads[numberOfFunctions] = thread(&TaskSynchronizator::checkKeyCancelation,this);
		threads[numberOfFunctions].detach();
	}
	else {

		threads[numberOfFunctions] = thread(&TaskSynchronizator::showPrompt, this,2);
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
			std::cout << "Functions which weren`t calculated : " << std::endl;
			int size = functions.size();
			for (int i = 0; i < size; i++) {
				if (functionResults[i] == -123) {
					std::cout << functions[i].second << std::endl;
				}
			}
		}
	}
}

void TaskSynchronizator::calculate(){
	if (operationType == OperationType::Mult)
		calculateMult();
}

void TaskSynchronizator::checkKeyCancelation()
{
	char choice = 65;
	do {
		//while (!_kbhit()) {}
		choice = _getch();
	} while (choice != 27);
	if (!workIsFinished)
	{
		workIsFinished = true;
		stopAllThreads();
		cv.notify_one();
	}
}

void TaskSynchronizator::showPrompt(int interval)
{
	if (!workIsFinished){
		std::this_thread::sleep_for(std::chrono::seconds(interval));
		if (!workIsFinished) {
			std::cout << "Enter option : \n"
				"1.Continue calculation.\n"
				"2.Continue without prompt.\n"
				"3.Cancel calculation. \n";
			char choice = 65;
			do {
				std::cin >> choice;
			} while (choice != '1' && choice != '2' && choice != '3');

			if (!workIsFinished) {
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
		//threads[i].~thread();
		TerminateThread(threads[i].native_handle(), 0);
	
	}

}
