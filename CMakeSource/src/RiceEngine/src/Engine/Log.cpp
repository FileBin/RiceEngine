#include "pch.h"
#include <GameEngine/Log.h>
#include <ios>
#include <locale.h>
#include <GameEngine/Util.h>
#include <GameEngine/Util/exceptions.h>

#define LOGNAME "log.txt"

namespace Game {

	Log* Log::instance = nullptr;
	Log::Localization Log::locale = {};

	void Log::PreInit() {
		if (!instance) {
			instance = new Log();
		} else
			log(LogLevel::ERR, locale.log_is_already_created.c_str());
	}

	Log::Log() {
		init();
	}

	Log::~Log() {
		close();
		instance = nullptr;
	}

	void Log::Close() {
		delete instance;
	}

	void Log::init() {
		file.open(LOGNAME, std::wfstream::out | std::wfstream::trunc);
		std::locale utf8("en_US.UTF-8");
		file.imbue(utf8);
		if (file.is_open()) {
			wchar_t timer[9];
			_wstrtime_s(timer, 9);
			wchar_t date[9];
			_wstrdate_s(date, 9);
			file << std::format(L"{}: {} {}.\n---------------------------------------\n\n", locale.log_begin, date, timer);
		} else {
			wprintf(locale.log_creation_error.c_str());
		}
	}

	void Log::close() {
		if (!file)
			return;

		wchar_t timer[9];
		_wstrtime_s(timer, 9);
		wchar_t date[9];
		_wstrdate_s(date, 9);
		file << std::format(L"\n---------------------------------------\n{}: {} {}", locale.log_end, date, timer);
		file.close();
	}
	
	/*void Print(String message, ...) {
		auto s = message.c_str();
		va_list args;

		va_start(args, s);
		int len = _vcwprintf(s, args) + 1;
		wchar_t* buffer = static_cast<wchar_t*>(malloc(len * sizeof(wchar_t)));
		_vswprintf_c(buffer, len, s, args);
		instance->print(L"", buffer);
		va_end(args);
		free(buffer);
	}

	void Log::Debug(String message, ...) {
#ifdef _DEBUG
		auto s = message.c_str();
		va_list args;
		va_start(args, s);
		int len = _vcwprintf(s, args) + 1;
		wchar_t* buffer = static_cast<wchar_t*>(malloc(len * sizeof(wchar_t)));
		_vswprintf_c(buffer, len, s, args);
		instance->print(L"DEBUG: ", buffer);
		va_end(args);
		free(buffer);
#endif
	}
	void Log::Err(String message, ...) {
		auto s = message.c_str();
		va_list args;
		va_start(args, s);
		int len = _vcwprintf(s, args) + 1;
		wchar_t* buffer = static_cast<wchar_t*>(malloc(len * sizeof(wchar_t)));
		_vswprintf_c(buffer, len, s, args);
		instance->print(L"ERROR: ", buffer);
		va_end(args);
		free(buffer);
	}*/

	void Log::print(String levtext, String text) {
		wchar_t timer[9];
		_wstrtime_s(timer, 9);
		clock_t cl = clock();

		auto str = std::format(L"{}::{}: [{}] {}\n", timer, cl, levtext, text);

		wprintf(str.c_str());
		file << str;
		file.flush();
	}
}