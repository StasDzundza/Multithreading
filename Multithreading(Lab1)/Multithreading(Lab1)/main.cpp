#include"TaskSynchronizator.h"
#include<exception>
#include"Functions.h"

using namespace std;

int main() {
	TaskSynchronizator* task = new TaskSynchronizator;
	task->setCancelationType(TaskSynchronizator::CancelationType::Esc_Key);
	for (int i = 0; i < 6; i++)
	{
		int x;
		std::cout << "Enter the x argument " << std::endl;	
		std::cin.clear();
		std::cin >> x;
		task->setX(x);
		task->addFunction(spos::f_func<spos::INT>);
		task->addFunction(spos::g_func<spos::INT>);
		task->start();
		task->clearSettings();
	}
	delete task;
	return 0;
}