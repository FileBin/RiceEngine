#pragma once
#include "stdafx.h"
#include <xnamath.h>

typedef std::vector<byte> data_t;
typedef std::wstring String;
typedef std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout;
typedef ID3D11Buffer Buffer;

struct Vector2;
namespace Game::Util {
	void ThrowIfFailed(HRESULT hr);
	data_t ReadFile(String filename);
	String Utf8ToWstring(const std::string& str);
	Vector2 GetWindowScreenSize(HWND hwnd);
}