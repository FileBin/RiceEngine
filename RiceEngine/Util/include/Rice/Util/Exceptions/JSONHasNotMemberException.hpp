#pragma once

#include <BetterCpp/Exceptions/Exception.hpp>

#include "../../Util.hpp"

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

		String info{};
		info.append(L"JSON File: ");
		info.append(GetFile());
		info.append(L"\n");

		info.append(L"Member: ");
		info.append(GetMember());
		info.append(L"\n");

		SetInfo(info);
	}

	String GetMember() { return member; }
	String GetFile() { return file; }
};

NSP_ENGINE_END
