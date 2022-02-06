#pragma once
#include "stdafx.h"
#include <fstream>
#include "Util.h"

namespace Game {

	static String logLevel_str[6] = { L"", L"INFO", L"DEBUG", L"WARNING", L"ERROR", L"CRITICAL_ERROR" };

	class Log {
		struct Localization {
			String log_begin = L"Log Created";
			String log_end = L"Log End";
			String log_is_already_created = L"Log is already created!";
			String log_creation_error = L"Log creation-error!";
		} static locale;

	public:
	
		static void PreInit();

		enum LogLevel {NONE = 0, INFO = 1, Debug = 2, WARNING = 3, ERR = 4, CRITICAl = 5};
		
		template<class... _Types> 
		static void log(LogLevel logLevel, String message, const _Types &...args) {
			auto str = std::format(message, args...);
			if (logLevel == LogLevel::Debug) {
#ifdef _DEBUG
				instance->print(logLevel_str[(int)logLevel], str);
#endif
			}
			else {
				instance->print(logLevel_str[(int)logLevel], str);
			}
		}

		static void Close();
		~Log();
	private:
		static Log* instance;
		Log();
		void init();
		void close();
		void print(String levtext, String text);

		std::wfstream file;
	};
}