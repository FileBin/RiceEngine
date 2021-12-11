#pragma once
#pragma warning( disable : 4005 4838 4267 4244 )

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
#include <D3DX11.h>
#include <DXGI.h>

#define ref &

#pragma comment(lib, "d3d11.lib")
#ifdef _DEBUG
#	pragma comment(lib, "d3dx11d.lib")
#else
#	pragma comment(lib, "d3dx11.lib")
#endif

#pragma comment(lib, "dxgi.lib")