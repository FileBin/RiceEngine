#pragma once
#include "../stdafx.hpp"
#include <fstream>
#include "../Util.hpp"

NSP_ENGINE_BEGIN

static String logLevel_str[] = { L"", L"INFO", L"DEBUG", L"WARNING", L"ERROR", L"CRITICAL_ERROR" };

class Log {
	struct Localization {
		String log_begin = "Log Created";
		String log_end = "Log End";
		String log_is_already_created = "Log is already created!";
		String log_creation_error = "Log creation-error!";
	} static llocale;

public:

	static void init();

	enum LogLevel {
		None = 0, Info = 1, Debug = 2, Warning = 3, Error = 4, Critical = 5
	};

	template<class ... _Types>
	static void log(LogLevel logLevel, String message, const _Types &...args) {
		if (instance == nullptr)
			return;
		auto str = String::format(message, args...);
		if (logLevel == LogLevel::Debug) {
#ifdef _DEBUG
			instance->print(logLevel_str[(int)logLevel], str);
#endif
		} else {
			instance->print(logLevel_str[(int) logLevel], str);
		}
	}

	static void log(String message, LogLevel logLevel = Info) {
		if (instance == nullptr)
			return;
		if (logLevel == LogLevel::Debug) {
#ifdef _DEBUG
			instance->print(logLevel_str[(int)logLevel], message);
#endif
		} else {
			instance->print(logLevel_str[(int) logLevel], message);
		}
	}

	template<class ... _Types>
	static void debug(String message, const _Types &...args) {
#ifdef _DEBUG
		if (instance == nullptr)
					return;
		log(Debug, message, args...);
#endif
	}

	static void close();
	~Log();
private:
	static RefPtr<Log> instance;
	Log();
	void _init();
	void _close();
	void print(String levtext, String text);

	std::wfstream file;
};

NSP_ENGINE_END
