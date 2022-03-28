#pragma once

#include "exception.h"

#define THROW_OPENAL_EXCEPTION(wtf) throw Game::OpenALException(wtf, __LINE__, __FILE__)

namespace Game {
	class OpenALException : public Exception {
	public:
		OpenALException(const char* what, int line_, const char* file_) : Exception(what, line_, file_) {}
	};
}

