#include "pch.h"
//#include <codecvt>

#include <Rice/Util.hpp>

NSP_UTIL_BEGIN

void checkPath(String path) {
	if (!std::filesystem::exists(path.toUtf8())) { // @suppress("Function cannot be resolved")
		THROW_FILE_NOT_FOUND_EXCEPTION(path);
	}
}

data_t readFile(String filename) {
	checkPath(filename);
	std::ifstream infile(filename.toUtf8(), std::ios_base::binary);
	auto data = data_t(std::istreambuf_iterator<char>(infile),
			std::istreambuf_iterator<char>());
	return data;
}

NSP_UTIL_END
