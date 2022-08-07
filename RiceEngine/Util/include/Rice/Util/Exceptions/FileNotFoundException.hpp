#pragma once

#include "Exception.hpp"
#include "Rice/Util/namespaces.h"

#define THROW_FILE_NOT_FOUND_EXCEPTION(file_name) throw NSP_UTIL::FileNotFoundException(file_name, __LINE__, __FILE__)

NSP_UTIL_BEGIN

class FileNotFoundException : public Exception {
public:
	FileNotFoundException(String file_name, int line_, const char* file_);
};

NSP_UTIL_END
