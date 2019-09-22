#include"TaskSynchronizator.h"
#include<exception>

using namespace std;

int main() {
	TaskSynchronizator* task = new TaskSynchronizator;
	
	int x;
	std::cout << "Enter the x argument " << std::endl;
	std::cin >> x;
	task->setX(x);

	task->addFunction([](int x) {
		std::this_thread::sleep_for(chrono::seconds(3));
		return 15 * x; });

	task->addFunction([](int x) {
		std::this_thread::sleep_for(chrono::seconds(10));
		return 140 * x + 126; });

	task->addFunction([](int x) {
		std::this_thread::sleep_for(chrono::seconds(7));
		return 150 * x - 25; });

	task->start();
	delete task;

	return 0;
}