#pragma once

#include <exception>
#include <string>

#define THROW_EXCEPTION(wtf) throw Game::Exception(wtf, __LINE__, __FILE__)

namespace Game {
    class Exception : public std::exception {
    private:
        int line;
        const char* file;
        char* stack;
        char* info;

        std::string getStack() {
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
                strstack += std::format("[{}]\t{} {:#x} at line {}\n", frames - i - 1, symbol->Name, symbol->Address, line.LineNumber);//'\n' << frames - i - 1 << ':' << symbol->Name << " " << (void*)symbol->Address;
            }

            free(symbol);
            return strstack;
        }

    public:
        Exception(const char* msg, int line_, const char* file_) : std::exception(msg), line(line_), file(file_) {
            auto s = getStack();
            auto size = s.size() * sizeof(char);
            stack = (char*)malloc(size + 1);
            info = (char*)malloc(7);
            memcpy((void*)stack, s.c_str(), size);
            stack[size] = '\0';
            memcpy((void*)info, "[NONE]", 7);
        }

        int GetLine() const { return line; }
        const char* GetFile() const { return file; }
        const char* GetStack() const { return stack; }
        const char* GetInfo() { return info; }

    protected:
        void SetInfo(const char* info_) {
            auto len = strlen(info_);
            memcpy((void*)info, (void*)info_, len);
        }
    };
}