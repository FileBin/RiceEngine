#pragma once

#include "exception.h"

#define THROW_INDEX_OUT_OF_RANGE_EXCEPTION throw Game::IndexOutOfRangeException(__LINE__, __FILE__)

namespace Game {
	class IndexOutOfRangeException : public Exception {
	public:
		IndexOutOfRangeException(int line_, const char* file_) : Exception("Index was out of range!", line_, file_) {}
	};
}