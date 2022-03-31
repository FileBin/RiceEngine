#pragma once

#include "Exception.hpp"

#define THROW_FILE_NOT_FOUND_EXCEPTION(file_name) throw ::Rice::FileNotFoundException(file_name, __LINE__, __FILE__)

NSP_ENGINE_BEGIN

class FileNotFoundException : public Exception {
public:
	FileNotFoundException(const char* file_name, int line_, const char* file_) : Exception("File not found!", line_, file_) {
		std::string info{};
		info.append("File Name: ");
		info.append(file_name);
		info.append("\n");
		SetInfo(info.c_str());
	}
};

NSP_ENGINE_END