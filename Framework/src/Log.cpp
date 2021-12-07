#include <GameFramework/Log.h>
#include <ios>
#include <locale.h>

#define LOGNAME "log.txt"

namespace GameFramework {

	Log* Log::instance = nullptr;

	void Log::Init() {
		if (!instance) {
			instance = new Log();

		} else
			Err(L"Log has been already created!");
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
			file << std::format(L"Log created: %s %s.\n---------------------------------------\n\n", date, timer);
		} else {
			wprintf(L"Log creation error...\n");
		}
	}

	void Log::close() {
		if (!file)
			return;

		wchar_t timer[9];
		_wstrtime_s(timer, 9);
		wchar_t date[9];
		_wstrdate_s(date, 9);
		file << std::format(L"\n---------------------------------------\nLog End: {} {}", date, timer);
		file.close();
	}

	void Log::Print(const wchar_t* message, ...) {
		va_list args;
		va_start(args, message);
		int len = _vcwprintf(message, args) + 1;
		wchar_t* buffer = static_cast<wchar_t*>(malloc(len * sizeof(wchar_t)));
		_vswprintf_c(buffer, len, message, args);
		instance->print(L"", buffer);
		va_end(args);
		free(buffer);
	}

	void Log::Debug(const wchar_t* message, ...) {
#ifdef _DEBUG
		va_list args;
		va_start(args, message);
		int len = _vcwprintf(message, args) + 1;
		wchar_t* buffer = static_cast<wchar_t*>(malloc(len * sizeof(wchar_t)));
		_vswprintf_c(buffer, len, message, args);
		instance->print(L"DEBUG: ", buffer);
		va_end(args);
		free(buffer);
#endif
	}
	void Log::Err(const wchar_t* message, ...) {
		va_list args;
		va_start(args, message);
		int len = _vcwprintf(message, args) + 1;
		wchar_t* buffer = static_cast<wchar_t*>(malloc(len * sizeof(wchar_t)));
		_vswprintf_c(buffer, len, message, args);
		instance->print(L"ERROR: ", buffer);
		va_end(args);
		free(buffer);
	}

	void Log::print(const wchar_t* levtext, const wchar_t* text) {
		wchar_t timer[9];
		_wstrtime_s(timer, 9);
		clock_t cl = clock();

		auto str = std::format(L"{}::{}: {}{}\n", timer, cl, levtext, text);

		wprintf(str.c_str());
		file << str;
		file.flush();
	}
}