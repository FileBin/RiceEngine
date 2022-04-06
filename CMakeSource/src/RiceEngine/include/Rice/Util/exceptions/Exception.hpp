#pragma once

#include "../../stdafx.hpp"

#include "../String.hpp"

#include <exception>

#define THROW_EXCEPTION(wtf) throw ::Rice::Exception(wtf, __LINE__, __FILE__)

NSP_ENGINE_BEGIN

class Exception : public std::exception {
private:
    int line;
    String file, msg;
    String stack;
    String info;

    String getStack() {
#ifdef _MSC_VER
        HANDLE process = GetCurrentProcess();
        SymInitialize(process, NULL, TRUE);

        void* stack[64];
        unsigned short frames = CaptureStackBackTrace(0, 64, stack, NULL);
        SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
        if (symbol == nullptr)
            throw std::runtime_error("MemoryError"); //Memory alloc excep
        symbol->MaxNameLen = 255;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

        std::string strstack{};
        for (int i = 0; i < frames; i++) {
            SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
            IMAGEHLP_LINE64 line;
            ZeroMemory(&line, sizeof(line));
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            DWORD dis;
            SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &dis, &line);
            strstack += fmt::format("[{}]\t{} at line {}, address: {:#x}\n", frames - i - 1, symbol->Name, line.LineNumber, symbol->Address);//'\n' << frames - i - 1 << ':' << symbol->Name << " " << (void*)symbol->Address;
        }

        free(symbol);
        return strstack;
#else
        //TODO make get stack
        return "";
#endif
    }

public:
    Exception(const char* msg_, int line_, const char* file_) : _STD exception(), msg(msg_), line(line_), file(file_) {
        stack = getStack();
    }

    int GetLine() const { return line; }
    String GetFile() const { return file; }
    String GetStack() const { return stack; }
    String GetInfo() { return info; }
    String GetMsg() { return msg; }

protected:
    void SetInfo(String info_) {
    	info = info_;
    }
};

NSP_ENGINE_END
