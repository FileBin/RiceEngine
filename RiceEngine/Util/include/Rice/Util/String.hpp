/*
 * String.hpp
 *
 *  Created on: 31 March 2022
 *      Author: FileBinsLapTop
 */
#include "stdafx.hpp"
#include <string>

struct String;
struct IPrintable;

#pragma once

struct IPrintable {
    virtual String toString() = 0;
    virtual ~IPrintable() {}
};

struct String : public std::wstring {
    String() : std::wstring() {}
    String(const String &str) : std::wstring(str) {}
    String(const char *pstr)
        : std::wstring(
              std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(
                  pstr)) {}
    String(const wchar_t *pwstr) : std::wstring(pwstr) {}
    String(const std::string &str)
        : std::wstring(
              std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(
                  str.c_str())) {}
    String(const std::wstring &wstr) : std::wstring(wstr) {}
    //String toString() override { return *this; }

    const String &operator=(wchar_t ch) {
        *this = String();
        at(0) = ch;
        return *this;
    }

    const String &operator=(const String& other) {
        static_cast<std::wstring*>(this)->assign(static_cast<const std::wstring&>(other));
        return *this;
    }

    std::string toUTF8String() {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
        return utf8_conv.to_bytes(*this);
    }
};

inline String operator+(const char *cstr, String s) { return String(cstr) + s; }

inline String operator+(String s, const char *cstr) { return s + String(cstr); }

inline String operator+(String s, std::string str) { return s + str.c_str(); }

inline String operator+(std::string s, String str) { return s.c_str() + str; }