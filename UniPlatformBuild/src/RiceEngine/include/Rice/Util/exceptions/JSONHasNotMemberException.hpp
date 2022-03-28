#pragma once

#include "../../Util.hpp"
#include "Exception.hpp"

#define THROW_JSON_HAS_NOT_MEMBER_EXCEPTION(JsonFile, member) throw ::Rice::JSONHasNotMemberException(JsonFile, member, __LINE__, __FILE__)

NSP_ENGINE_BEGIN

class JSONHasNotMemberException : public Exception {
private:
	const char* member;
	const wchar_t* file;
public:
	JSONHasNotMemberException(const wchar_t* jsonFile, const char* member_, int line_, const char* file_) : Exception("Json Hasn't member!", line_, file_) {
		file = jsonFile;
		member = member_;

		std::string info{};
		info.append("JSON File: ");
		info.append(Util::ToStdString(GetFile()));
		info.append("\n");

		info.append("Member: ");
		info.append(GetMember());
		info.append("\n");

		SetInfo(info.c_str());
	}

	const char* GetMember() { return member; }
	const wchar_t* GetFile() { return file; }
};

NSP_ENGINE_END