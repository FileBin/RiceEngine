#include "Log.h"

#include "stdafx.h"
#include "Log.h"

#define LOGNAME "log.txt"

namespace GameFramework {
	//------------------------------------------------------------------

	Log* instance = nullptr;

	void Log::Init() {
		if (!instance) {
			instance = new Log();

		} else
			Err(L"Log уже был создан");
	}

	Log::Log() {
		file = nullptr;
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
		if (fopen_s(&file, LOGNAME, "w") == 0) {
			wchar_t timer[9];
			_wstrtime_s(timer, 9);
			wchar_t date[9];
			_wstrdate_s(date, 9);
			_fwprintf_p(file, L"Log created: %s %s.\n---------------------------------------\n\n", date, timer);
			//_fwprintf_p(file, L"---------------------------------------\n\n");
		} else {
			wprintf(L"Ошибка при создании файла лога...\n");
			file = nullptr;
		}
	}

	void Log::close() {
		if (!file)
			return;

		wchar_t timer[9];
		_wstrtime_s(timer, 9);
		wchar_t date[9];
		_wstrdate_s(date, 9);
		_fwprintf_p(file, L"\n---------------------------------------\nEnd of Log: %s %s.", date, timer);
		fclose(file);
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

		wprintf(L"%s::%d: %s%s\n", timer, cl, levtext, text);
		if (file) {
			_fwprintf_p(file, L"%s::%d: %s%s\n", timer, cl, levtext, text);
			fflush(file);
		}
	}

	//------------------------------------------------------------------
}