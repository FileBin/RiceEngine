#pragma once
#include "stdafx.hpp"

struct Vector2;

NSP_UTIL_BEGIN
void CheckPath(String path);
data_t ReadFile(String filename);
String Utf8ToWstring(const std::string& str);
std::string ToStdString(const String& s);
//D2D1::ColorF GetColByHex(uint hex);
NSP_UTIL_END