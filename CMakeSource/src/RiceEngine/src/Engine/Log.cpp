#include "pch.h"
#include <Rice/Engine/Log.hpp>
#include <ios>
#include <locale.h>
#include <iostream>

#define LOGNAME "log.txt"

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
    auto utf8 = locale(locale(), new codecvt_utf8<wchar_t>);
	file.imbue(utf8);
	if (file.is_open()) {
		wchar_t timer[9];
		_wstrtime_s(timer, 9);
		wchar_t date[9];
		_wstrdate_s(date, 9);
		file << fmt::format(
						L"{}: {} {}.\n---------------------------------------\n\n",
						llocale.log_begin, date, timer);
	} else {
		wprintf(llocale.log_creation_error.c_str());
	}
}

void Log::_close() {
	if (!file)
		return;

	wchar_t timer[9];
	_wstrtime_s(timer, 9);
	wchar_t date[9];
	_wstrdate_s(date, 9);
	file << fmt::format(
					L"\n---------------------------------------\n{}: {} {}",
					llocale.log_end, date, timer);
	file.close();
}

void Log::print(String levtext, String text) {
	wchar_t timer[9];
	_wstrtime_s(timer, 9);
	clock_t cl = clock();

	String str = fmt::format(L"{}::{}: [{}] {}\n", timer, cl, levtext, text);

	_STD wcout << str.c_str();
	fflush(stdout);
	file << str;
#ifdef _DEBUG
	file.flush();
#endif
}

NSP_ENGINE_END
