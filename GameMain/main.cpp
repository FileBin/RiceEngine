#include "pch.h"

#include <GameEngine/Core.h>
#include <Scripts/PreInitScript.h>

using namespace Game;
void printStack(void) {
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);

    void* stack[64];
    unsigned short frames = CaptureStackBackTrace(0, 64, stack, NULL);

    SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    std::cout << "Backtrace:";
    for (int i = 0; i < frames; i++) {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        std::cout << '\n' << frames - i - 1 << ':' << symbol->Name << " " << (void*)symbol->Address;
    }

    free(symbol);
}

int main(int argc, char** argv[]) {
    printStack();


#ifndef _DEBUG
	ShowWindow(GetConsoleWindow(), 0);
#else
	ShowWindow(GetConsoleWindow(), 1);
#endif

	SetProcessDPIAware();
	Core::RunNew(new PreInitScript());
	ShowWindow(GetConsoleWindow(), 0);

	return 0;
}