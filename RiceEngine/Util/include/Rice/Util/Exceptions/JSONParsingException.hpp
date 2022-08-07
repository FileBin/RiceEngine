#pragma once

#include <BetterCpp/Exceptions/Exception.hpp>

#include "../../Util.hpp"

#define THROW_JSON_PARSING_EXCEPTION(JsonFile, offset) throw ::Rice::JSONParsingException(JsonFile, offset, __LINE__, __FILE__)

NSP_ENGINE_BEGIN

class JSONParsingException : public Exception {
private:
	size_t offset = 0;
	const wchar_t* file;
public:
	JSONParsingException(const wchar_t* jsonFile, size_t offset_, int line_, const char* file_) : Exception("Json Parsing Error!", line_, file_) {
		offset = offset_;
		file = jsonFile;

		String info{};
		info.append(L"JSON File: ");
		info.append(GetFile());
		info.append(L"\n");

		info.append(L"Offset: ");
		info.append(std::to_wstring(GetOffset()));
		info.append(L"\n");

		SetInfo(info.c_str());
	}

	size_t GetOffset() { return offset; }
	String GetFile() { return file; }
};

NSP_ENGINE_END
