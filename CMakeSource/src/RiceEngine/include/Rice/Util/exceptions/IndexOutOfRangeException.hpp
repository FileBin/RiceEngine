#pragma once

#include "Exception.hpp"

#define THROW_INDEX_OUT_OF_RANGE_EXCEPTION throw ::Rice::IndexOutOfRangeException(__LINE__, __FILE__)

NSP_ENGINE_BEGIN
	class IndexOutOfRangeException : public Exception {
	public:
		IndexOutOfRangeException(int line_, const char* file_) : Exception("Index was out of range!", line_, file_) {}
	};
NSP_ENGINE_END