#pragma once

#include "Exception.hpp"
#include "Rice/Util/namespaces.h"

#define THROW_UNKNOWN_FORMAT_EXCEPTION throw NSP_UTIL::UnknownFormatException(__LINE__, __FILE__)

NSP_UTIL_BEGIN

class UnknownFormatException : public Exception {
public:
	UnknownFormatException(int line_, const char* file_) : Exception("Format is Unknown", line_, file_) {}
};

NSP_UTIL_END

