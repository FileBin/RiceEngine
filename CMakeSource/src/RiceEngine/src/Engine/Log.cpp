#include "pch.h"
#include <Rice/Engine/Log.hpp>
#include <ios>
#include <locale.h>
#include <iostream>

#define LOGNAME "log.txt"

String get_date() {
	return "";
}

String get_timer() {
	return "";
}

NSP_ENGINE_BEGIN

SmartPtr<Log> Log::instance = nullptr;
Log::Localization Log::llocale = { };

void Log::init() {
	if (instance.isNull()) {
		instance = new Log();
	} else
		log(LogLevel::Error, llocale.log_is_already_created.c_str());
}

Log::Log() {
	_init();
}

Log::~Log() {
	_close();
}

void Log::close() {
	instance.release();
}

void Log::_init() {
	using namespace ::std;
	file.open(LOGNAME, wfstream::out | wfstream::trunc);
    auto utf8 = locale(locale(), new codecvt_utf8<wchar_t>()); // @suppress("Type cannot be resolved") // @suppress("Symbol is not resolved")
	file.imbue(utf8);
	if (file.is_open()) {
		file << fmt::format( // @suppress("Invalid arguments")
						L"{}: {} {}.\n---------------------------------------\n\n",
						llocale.log_begin, get_date(), get_timer());
	} else {
		wprintf(llocale.log_creation_error.c_str());
	}
}

void Log::_close() {
	if (!file)
		return;

	file << fmt::format( // @suppress("Invalid arguments")
					L"\n---------------------------------------\n{}: {} {}",
					llocale.log_end, get_date(), get_timer());
	file.close();
}

void Log::print(String levtext, String text) {
	clock_t cl = clock();

	String str = fmt::format(L"{}::{}: [{}] {}\n", get_timer(), cl, levtext, text); // @suppress("Invalid arguments")

	_STD wcout << str.c_str();
	fflush(stdout);
	file << str;
#ifdef _DEBUG
	file.flush();
#endif
}

NSP_ENGINE_END
