#pragma once
#include "stdafx.h"
#include <fstream>
#include "Util.h"

namespace Game {
	class Log {
	public:
		struct Localization {
			String log_begin;
			String log_end;
			String log_is_already_created;
			String log_creation_error;

			static Localization LoadFromJSON(String filename);
		} static locale;
	
		static void Init();
		static void LoadLocalization(Localization locale);

		template<class... _Types> 
		static void Print(String message, const _Types &...args) {
			auto str = std::format(message, args...);
			instance->print(L"", str);
		}
#ifdef _DEBUG
		template<class... _Types>
		static void Debug(String message, const _Types &...args) {

			auto str = std::format(message, args...);
			instance->print(L"DEBUG:", str);
		}
#else
		static void Debug(String message, ...) {}
#endif

		template<class... _Types>
		static void Err(String message, const _Types &...args) {
			auto str = std::format(message, args...);
			instance->print(L"ERROR:", str);
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

