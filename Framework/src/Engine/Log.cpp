#include <GameEngine/Log.h>
#include <ios>
#include <locale.h>
#include <GameEngine/Util.h>
#include <rapidjson/document.h>

#define LOGNAME "log.txt"

namespace Game {

	Log* Log::instance = nullptr;
	Log::Localization Log::locale = {};

	void Log::Init() {
		if (!instance) {
			instance = new Log();
		} else
			log(LogLevel::ERR, locale.log_is_already_created.c_str());
	}

	void Log::LoadLocalization(Localization locale) { Log::locale = locale; }

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

	Log::Localization Log::Localization::LoadFromJSON(String filename) {
		using namespace rapidjson;
		Localization loc = {};
		auto fileData = Util::ReadFile(filename);
		fileData.push_back('\0');

		Document doc;
		doc.Parse((const char*)fileData.data());
		try {
			if (doc.HasParseError()) throw -1;

			const char* values[] = { "log.begin", "log.end", "log.already_created", "log.creation_error" };
			std::function<void(String)> functions[] = {
				[&](String v) { loc.log_begin = v; },
				[&](String v) { loc.log_end = v; },
				[&](String v) { loc.log_is_already_created = v; },
				[&](String v) { loc.log_creation_error = v; },
			};

			auto n = ARRAYSIZE(values);
			for (size_t i = 0; i < n; i++) {
				auto v = values[i];
				if (doc.HasMember(v)) {
					functions[i](Util::Utf8ToWstring(doc[v].GetString()));
				} else throw -2;
			}
		} catch (int code) {
			if (code == -1) {
				throw std::exception("JSONParsingError!");
			} else if(code == -2) {
				throw std::exception("JSONHasNotMemberError!");
			}
		}

		return loc;
	}
}