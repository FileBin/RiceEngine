// ModelConverter.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "src/functions.h"

using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Specify Model Name!";
		return 0;
	}
	string modelName = argv[1];
	cout << "loading " << modelName << "...\n";
	auto root = getRootNode(modelName);
	PrintNode(root);
	return 0;
}
