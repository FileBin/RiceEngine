#pragma once
#include "stdafx.h"
namespace GameFramework {
	class Log {
	public:
		static void Init();
		static void Print(const wchar_t* message, ...);
		static void Debug(const wchar_t* message, ...);
		static void Err(const wchar_t* message, ...);
		static void Close();
		~Log();
	private:
		//static Log* instance;
		Log();
		void init();
		void close();
		void print(const wchar_t* levtext, const wchar_t* text);

		FILE* file;
	};
}

