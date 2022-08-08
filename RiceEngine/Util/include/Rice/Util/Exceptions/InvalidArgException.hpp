#pragma once

#include "Exception.hpp"
#include "Rice/Util/namespaces.h"

#define THROW_INVALID_ARG_EXCEPTION(arg) throw NSP_UTIL::InvalidArgException(arg, __LINE__, __FILE__)

NSP_UTIL_BEGIN

class InvalidArgException : public Exception {
public:
	InvalidArgException(const char* arg, int line_, const char* file_) : Exception("Argument was Invalid!", line_, file_) {
		std::string info{};
		info.append("Argument Name: ");
		info.append(arg);
		info.append("\n");
		setInfo(info.c_str());
	}
};

NSP_UTIL_END