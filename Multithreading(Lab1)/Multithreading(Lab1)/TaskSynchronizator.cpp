#include "TaskSynchronizator.h"
#include <numeric>
#include <functional>
#include <chrono>
#include <Windows.h>
using std::cout;
using std::endl;

TaskSynchronizator::TaskSynchronizator() {
	oneOfResultsIsZero = false;
	workIsFinished = false;
	inputIsWaiting = false;
	promptIsShowing = false;
	cancelChoosed = false;
	continueChoosed = false;
	oneOfResultsIsReady = false;
	operationType = TaskSynchronizator::OperationType::Mult;
	cancelationType = TaskSynchronizator::CancelationType::Esc_Key;
	numberOfFunctions = 0;
	numberOfResults = 0;
	x = 0;
}

void TaskSynchronizator::addFunction(const function<int(int)>& f, const char* name) {
	functions.push_back(std::make_pair(std::move(f), name));
	++numberOfFunctions;
	functionResults.push_back(-123);
	calculatedFunctions.push_back(0);
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
			oneOfResultsIsReady = true;
			notifyAboutWork(functionResults[i]);
			});
		threads[i].detach();
	}
}

void TaskSynchronizator::notifyAboutWork(int resultOfCalculating){
	if (cancelChoosed) {
		stopAllThreads();
		return;
	}
	cv.notify_one();
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

	int result = 1;
	while (true) {
		std::unique_lock<mutex>locker(mtx);//mutex lock(constructor lock,destructor unlock)
		cv.wait(locker, [this]() { return (oneOfResultsIsReady) || cancelChoosed || continueChoosed; });//makes lock free,when sleeping
		//and locks when waking up
		continueChoosed = false;
		if (oneOfResultsIsReady) {
			for (int i = 0; i < functionResults.size(); i++){
				if (functionResults[i] != -123 && !calculatedFunctions[i]) {
					calculatedFunctions[i] = 1;
					result *= functionResults[i];
				}
			}
			oneOfResultsIsReady = false;
			numberOfFunctions--;
			cout << 1 << endl;
		}
	
		if (result == 0) {
			oneOfResultsIsZero = true;
			workIsFinished = true;
			cout << 0 << endl;
		}

		if (!numberOfFunctions || oneOfResultsIsZero || workIsFinished || cancelChoosed) {
			cout << "break" << numberOfFunctions << endl;
			workIsFinished = true;
			if(!promptIsShowing)
				break;
		}		
	}
	promptMtx.lock();
	if(inputIsWaiting)
		clearInput();

	if (!oneOfResultsIsZero && !numberOfFunctions && workIsFinished){
		std::cout << "Result of calculation is " << result << std::endl;
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
		inputIsWaiting = true;
		choice = _getch();
		inputIsWaiting = false;
	} while (choice != 27);

	cancelChoosed = true;

	if (!workIsFinished)
	{
		workIsFinished = true;
		stopAllThreads();
		cv.notify_one();
	}
}

void TaskSynchronizator::showPrompt(int interval)
{
	promptMtx.lock();
	if (!workIsFinished){
		std::this_thread::sleep_for(std::chrono::seconds(interval));
		//mtx.lock();
		if (!workIsFinished) {			
			promptIsShowing = true;
			std::cout << "Enter option : \n"
				"1.Continue calculation.\n"
				"2.Continue without prompt.\n"
				"3.Cancel calculation. \n";
			char choice = 65;
			do {
				inputIsWaiting = true;
				std::cin >> choice;
				promptIsShowing = false;
				inputIsWaiting = false;
			} while (choice != '1' && choice != '2' && choice != '3');
			//mtx.unlock();
			if (!workIsFinished) {
				switch (choice)
				{
				case '1': {
					continueChoosed = true;
					showPrompt(interval);
					break;
				}
				case '2': {
					continueChoosed = true;
					break;
				}
				case '3': {
					stopAllThreads();
					cancelChoosed = true;
					break;
				}
				}					
				cv.notify_one();
			}
			else {
				continueChoosed = true;
				cv.notify_one();
			}
		}			
	}
	promptMtx.unlock();	
}

void TaskSynchronizator::clearInput(){
	if (cancelationType == TaskSynchronizator::CancelationType::Esc_Key) {
		// Set up a generic keyboard event.
		INPUT ip;
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0; // hardware scan code for key
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		// Press the Esc key
		ip.ki.wVk = 0x1B; // virtual-key code for the Esc key
		ip.ki.dwFlags = 0; // 0 for key press
		SendInput(1, &ip, sizeof(INPUT));
	}
	else {
		// Set up a generic keyboard event.
		INPUT ip;
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0; // hardware scan code for key
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		// Press the 1 key
		ip.ki.wVk = 0x31; // virtual-key code for the 1 key
		ip.ki.dwFlags = 0; // 0 for key press
		SendInput(1, &ip, sizeof(INPUT));
		ip.ki.wVk = 0x0D;//Enter Key
		SendInput(1, &ip, sizeof(INPUT));
		_putch('\b');
		_putch(' ');
		_putch('\b');
	}
}

void TaskSynchronizator::calculateMult(){
	int result = std::accumulate(functionResults.begin(), functionResults.end(), 1, std::multiplies<int>());
	std::cout << "Result of calculation is " << result << std::endl;
}

void TaskSynchronizator::stopAllThreads()
{
	for (int i = 0; i < threads.size(); i++) {
		TerminateThread(threads[i].native_handle(), 0);
	}
}
