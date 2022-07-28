/*
 * String.hpp
 *
 *  Created on: 31 March 2022
 *      Author: FileBinsLapTop
 */
#include "stdafx.hpp"

struct String;
struct IPrintable;

#pragma once

struct IPrintable {
    virtual String toString() = 0;
    virtual ~IPrintable() {}
};

typedef std::wstring std_wstring;

struct String : public std_wstring, public IPrintable {
    String() : std_wstring() {}
    String(const wchar_t *wstr) : std_wstring(wstr, wcslen(wstr)) {}

    String(const char *cstr);

    String(const std_wstring &wstr) : std_wstring(wstr) {}
    String toString() override { return *this; }

    String operator=(wchar_t ch) {
        *this = String();
        at(0) = ch;
        return *this;
    }

    std::string toUTF8String() {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
        return utf8_conv.to_bytes(*this);
    }
};

inline String::String(const char *cstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    *this = utf8_conv.from_bytes(cstr);
}

inline String operator+(const char *cstr, String s) { return String(cstr) + s; }

inline String operator+(String s, const char *cstr) { return s + String(cstr); }

inline String operator+(String s, std::string str) { return s + str.c_str(); }

inline String operator+(std::string s, String str) { return s.c_str() + str; }