#include "pch.h"
//#include <codecvt>

#include <Rice/Util.hpp>

NSP_UTIL_BEGIN

void checkPath(String path) {
	if (!std::filesystem::exists(path.toUTF8String())) {
		THROW_FILE_NOT_FOUND_EXCEPTION(path.toUTF8String().c_str());
	}
}

data_t readFile(String filename) {
	checkPath(filename);
	std::ifstream infile(filename.toUTF8String(), std::ios_base::binary);
	auto data = data_t(std::istreambuf_iterator<char>(infile),
			std::istreambuf_iterator<char>());
	return data;
}

std::string ToStdString(const String& s) {
	std::string out {};
	for (int i = 0; s[i]; ++i) {
		wchar_t wc = s[i];
		char c = (char)wc;
		out.push_back(c);
	}
	return out;
}

NSP_UTIL_END
