#include "pch.h"
#include <Rice/Engine/Log.hpp>
#include <ios>
#include <locale.h>
#include <iostream>

#define LOGNAME "log.txt"

using namespace std::chrono;

steady_clock::time_point start;

String get_date() {
	using namespace std::chrono;
	auto t = std::time(0);
	std::tm time = *std::localtime(&t);
	wchar_t buf[0x100];
	wcsftime(buf,0x100, L"%X %x %Z", &time);
	return buf;
	//return std::to_string(std::put_time(&time, "%c %Z"));
	/*return fmt::format(L"{}:{}:{}, {:%A}, {} {}, {}", // @suppress("Invalid arguments")
			time->tm_hour, time->tm_min, time->tm_sec,
			time->tm_wday,
			time->tm_mday, time->tm_mon,
			time->tm_year); */
}

String get_timer() {
	double time = duration_cast<microseconds>(steady_clock::now() - start).count() / 1000. / 1000.;
	return fmt::format(L"{:.4f}s", time); // @suppress("Invalid arguments")
}

NSP_ENGINE_BEGIN

RefPtr<Log> Log::instance = nullptr;
Log::Localization Log::llocale = { };

void Log::init() {
	if (instance.isNull()) {
		instance = RefPtr<Log>{new Log()};
	} else
		log(LogLevel::Error, llocale.log_is_already_created);
}

Log::Log() {
	start = std::chrono::steady_clock::now();
	_init();
}

Log::~Log() {
	_close();
}

void Log::close() {
	instance.release();
}

void Log::_init() {
	using namespace std;
	file.open(LOGNAME, wfstream::out | wfstream::trunc);
#ifdef _MSC_VER
	auto utf8 = std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>);
#else
	auto utf8 = locale(file.getloc(), new codecvt_utf8<wchar_t>()); // @suppress("Type cannot be resolved") // @suppress("Symbol is not resolved")
#endif // _MSC_VER

	file.imbue(utf8);
	if (file.is_open()) {
		file << String::format( // @suppress("Invalid arguments")
						L"{}: {}.\n---------------------------------------\n\n",
						llocale.log_begin, get_date()).toWide();
	} else {
		wprintf(llocale.log_creation_error.toWide());
	}
}

void Log::_close() {
	if (!file)
		return;

	file << String::format( // @suppress("Invalid arguments")
					L"\n---------------------------------------\n{}: {}\nTotal execution time: {}",
					llocale.log_end, get_date(), get_timer()).toWide();
	file.flush();
	file.close();
}

void Log::print(String levtext, String text) {

	String str = String::format("{}: [{}] {}\n", get_timer(), levtext, text); // @suppress("Invalid arguments")

	std::wstring wstr = str.toWide();

	std::wcout << wstr;
	fflush(stdout);
	file << wstr;
#ifdef _DEBUG
	file.flush();
#endif
}

NSP_ENGINE_END
