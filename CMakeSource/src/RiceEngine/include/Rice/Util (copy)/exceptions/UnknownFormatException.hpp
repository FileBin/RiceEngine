#pragma once

#include "Exception.hpp" 

#define THROW_UNKNOWN_FORMAT_EXCEPTION throw ::Rice::UnknownFormatException(__LINE__, __FILE__)

NSP_ENGINE_BEGIN

class UnknownFormatException : public Exception {
public:
	UnknownFormatException(int line_, const char* file_) : Exception("Format is Unknown", line_, file_) {}
};

NSP_ENGINE_END

