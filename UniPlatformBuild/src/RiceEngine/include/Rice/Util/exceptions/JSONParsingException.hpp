#pragma once

#include "../../Util.hpp"
#include "Exception.hpp"

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

		std::string info{};
		info.append("JSON File: ");
		info.append(Util::ToStdString(GetFile()));
		info.append("\n");

		info.append("Offset: ");
		info.append(std::to_string(GetOffset()));
		info.append("\n");

		SetInfo(info.c_str());
	}

	size_t GetOffset() { return offset; }
	const wchar_t* GetFile() { return file; }
};

NSP_ENGINE_END
