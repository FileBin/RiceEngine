#pragma once
#include "stdafx.h"

#include "Util/defines.h"

struct Vector2;

namespace Game::Util {
	void ThrowIfFailed(HRESULT hr);
	data_t ReadFile(String filename);
	String Utf8ToWstring(const std::string& str);
	Vector2 GetWindowScreenSize(HWND hwnd);
}