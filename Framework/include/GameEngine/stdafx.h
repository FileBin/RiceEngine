#pragma once

#include <clocale>
#include <ctime>
#include <stdexcept>
#include <format>
#include <fstream>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <functional>


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib, "d3d11.lib")
#ifdef _DEBUG
#	pragma comment(lib, "d3dx11d.lib")
#	pragma comment(lib, "DirectXTK_debug.lib")
#else
#	pragma comment(lib, "d3dx11.lib")
#	pragma comment(lib, "DirectXTK.lib")
#endif

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "ogg.lib")
#pragma comment(lib, "vorbis.lib")
#pragma comment(lib, "vorbisfile.lib")
#pragma comment(lib, "vorbisenc.lib")