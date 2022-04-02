#include "stdafx.hpp"

#pragma once

struct Vector2;

#include "Util/String.hpp"

NSP_UTIL_BEGIN
void CheckPath(String path);
data_t ReadFile(String filename);
String Utf8ToWstring(const std::string& str);
std::string ToStdString(String s);
NSP_UTIL_END
