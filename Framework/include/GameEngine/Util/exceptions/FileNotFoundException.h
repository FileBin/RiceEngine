#pragma once

#include "exception.h"

#define THROW_FILE_NOT_FOUND_EXCEPTION throw Game::FileNotFoundException(__LINE__, __FILE__)

namespace Game {
	class FileNotFoundException : public Exception {
	public:
		FileNotFoundException(int line_, const char* file_) : Exception("File not found!", line_, file_) {}
	};
}