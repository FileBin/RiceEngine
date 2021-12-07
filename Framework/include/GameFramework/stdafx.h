#pragma once
#pragma warning( disable : 4005 4838 )

#include <clocale>
#include <ctime>
#include <stdexcept>
#include <format>

#include <string>
#include <list>
#include <map>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>
#include <D3DX11.h>

#pragma comment(lib, "d3d11.lib")
#ifdef _DEBUG
#	pragma comment(lib, "d3dx11d.lib")
#else
#	pragma comment(lib, "d3dx11.lib")
#endif