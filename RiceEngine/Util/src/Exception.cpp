/*
 * Exception.cpp
 *
 *  Created on: May 1, 2022
 *      Author: filebin
 */

#include <fmt/format.h>
#include <fmt/xchar.h>

#include "Rice/Util/Util.hpp"

NSP_UTIL_BEGIN
struct exception_data {
	String stack = "";
	String info = "";
};
String Exception::getStack() {
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

	std::string strstack {};
	for (int i = 0; i < frames; i++) {
		SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
		IMAGEHLP_LINE64 line;
		ZeroMemory(&line, sizeof(line));
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
		DWORD dis;
		SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &dis, &line);
		strstack += fmt::format("[{}]\t{} at line {}, address: {:#x}\n", frames - i - 1, symbol->Name, line.LineNumber, symbol->Address); //'\n' << frames - i - 1 << ':' << symbol->Name << " " << (void*)symbol->Address;
	}

	free(symbol);
	return strstack;
#else
	//TODO make get stack
	return "";
#endif
}

Exception::Exception(const char* msg, int line, const char* file) : std::runtime_error(msg), _line(line), _file(file) {
	data = new exception_data;
	data->stack = getStack();
}

int Exception::line() const {
	return _line;
}
String Exception::file() const {
	return _file;
}
String Exception::stack() const {
	return data->stack;
}
String Exception::info() const{
	return data->info;
}
String Exception::msg() const{
	return what();
}

Exception::~Exception() {
	delete data;
}

void Exception::setInfo(String info) {
	data->info = info;
}

IndexOutOfRangeException::IndexOutOfRangeException(int line_, const char* file_) : Exception("Index was out of range!", line_, file_) {}

FileNotFoundException::FileNotFoundException(String file_name, int line, const char* file) : Exception("File not found!", line, file) {
	String info { };
	info.append(L"File Name: ");
	info.append(file_name);
	info.append(L"\n");
	setInfo(info);
}

void NullPtrException::__setInfo() {
	//String info = String::format(L"TypeInfo: {}\nPointer: {:#x}\n", String(getTypeInfo().name()), (ulong)getPointer());//fmt::format(L"TypeInfo: {}\nPointer: {:#x}\n", String(getTypeInfo().name()), (ulong)getPointer()); // @suppress("Invalid arguments")
	String info = fmt::format(L"TypeInfo: {}\nPointer: {:#x}\n", String(getTypeInfo().name()), (ulong)getPointer()); // @suppress("Invalid arguments")
	setInfo(info);
}

NSP_UTIL_END




