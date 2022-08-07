#pragma once

#include "Exception.hpp"
#include "Rice/Util/namespaces.h"

#define THROW_INDEX_OUT_OF_RANGE_EXCEPTION throw NSP_UTIL::IndexOutOfRangeException(__LINE__, __FILE__)

NSP_UTIL_BEGIN

class IndexOutOfRangeException : public Exception {
public:
	IndexOutOfRangeException(int line_, const char* file_);
};

NSP_UTIL_END
