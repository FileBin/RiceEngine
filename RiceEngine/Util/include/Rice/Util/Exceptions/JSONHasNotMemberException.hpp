#pragma once

#include "Exception.hpp"
#include "Rice/Util/namespaces.h"

#define THROW_JSON_HAS_NOT_MEMBER_EXCEPTION(JsonFile, member) throw NSP_UTIL::JSONHasNotMemberException(JsonFile, member, __LINE__, __FILE__)

NSP_UTIL_BEGIN

class JSONHasNotMemberException : public Exception {
private:
	const char* member;
	const wchar_t* file;
public:
	JSONHasNotMemberException(const wchar_t* jsonFile, const char* member_, int line_, const char* file_);

	String getMember();
	String getFile();
};

NSP_UTIL_END
