/*
 * String.hpp
 *
 *  Created on: 31 March 2022
 *      Author: FileBinsLapTop
 */
#include "../stdafx.hpp"

struct String;
struct IPrintable;

#pragma once

#include "IPrintable.hpp"

typedef _STD wstring std_wstring;

struct String : public std_wstring, public IPrintable {
    String() : std_wstring() {}
    String(const wchar_t* wstr) : std_wstring(wstr, wcslen(wstr)) {}

    String(const char* cstr);

    String(const std_wstring& wstr) : std_wstring(wstr) {}
    String toString() override {
        return *this;
    }

    String operator=(wchar_t ch) {
        *this = String();
        at(0) = ch;
        return *this;
    }
};

inline String::String(const char* cstr) {
    size_t size = strlen(cstr);
    resize(size);
    for (size_t pos = 0; pos < size; pos++) {
        at(pos) = cstr[pos];
    }
}

inline String operator+(const char* cstr, String s) {
    return String(cstr) + s;
}

inline String operator+(String s, const char* cstr) {
    return s + String(cstr);
}

inline String operator+(String s, _STD string str) {
    return s + str.c_str();
}

inline String operator+(_STD string s, String str) {
    return s.c_str() + str;
}
