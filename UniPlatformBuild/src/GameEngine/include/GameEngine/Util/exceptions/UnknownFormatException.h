#pragma once

#include "exception.h"

#define THROW_UNKNOWN_FORMAT_EXCEPTION throw Game::UnknownFormatException(__LINE__, __FILE__)

namespace Game {
	class UnknownFormatException : public Exception {
	public:
		UnknownFormatException(int line_, const char* file_) : Exception("Format is Unknown", line_, file_) {}
	};
}

