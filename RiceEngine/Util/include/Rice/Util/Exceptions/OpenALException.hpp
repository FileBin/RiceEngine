#pragma once

#include <BetterCpp/Exceptions/Exception.hpp>

#define THROW_OPENAL_EXCEPTION(wtf) throw ::Rice::OpenALException(wtf, __LINE__, __FILE__)

NSP_ENGINE_BEGIN

	class OpenALException : public Exception {
	public:
		OpenALException(const char* what, int line_, const char* file_) : Exception(what, line_, file_) {}
	};

NSP_ENGINE_END

