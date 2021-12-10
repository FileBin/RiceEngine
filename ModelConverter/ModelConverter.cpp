// ModelConverter.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>

int main(int argc, char* argv[])
{
	for (size_t i = 0; i < argc; i++) {
		std::cout << argv << "\n";
	}
}
