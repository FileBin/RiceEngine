#pragma once

#include "exception.h"

#define THROW_INVALID_ARG_EXCEPTION(arg) throw Game::InvalidArgException(arg, __LINE__, __FILE__)

namespace Game {
	class InvalidArgException : public Exception {
	public:
		InvalidArgException(const char* arg, int line_, const char* file_) : Exception("Argument was Invalid!", line_, file_) {
			std::string info{};
			info.append("Argument Name: ");
			info.append(arg);
			info.append("\n");
			SetInfo(info.c_str());
		}
	};
}