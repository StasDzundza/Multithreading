#include"TaskSynchronizator.h"
#include <exception>
#include "demofuncs"

using namespace std;

int main() {

	for (int i = 0; i < 6; i++)
	{
		TaskSynchronizator* task = new TaskSynchronizator;
		task->setCancelationType(TaskSynchronizator::CancelationType::Prompt);
		task->setOperationType(TaskSynchronizator::OperationType::Mult);
		int x;
		std::cout << "Enter the x argument " << std::endl;	
		std::cin >> x;
		task->setX(x);
		task->addFunction(spos::f_func<spos::INT>,"f(x)");
		task->addFunction(spos::g_func<spos::INT>,"g(x)");
		task->start();
		//delete task;якщо видаляємо об'єкт то потік який після відміни завершив роботу може повернути результат, а цей об'єкт буде пустим
	}
	
	return 0;
}