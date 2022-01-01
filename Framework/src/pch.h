#pragma once

#pragma warning( disable : 4005 4838 4267 4244 4458 )

#include <SpriteBatch.h>
#include <SpriteFont.h>

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

#include <wrl.h>
#include <atlbase.h>

#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib, "d3d11.lib")
#ifdef _DEBUG
#	pragma comment(lib, "d3dx11d.lib")
#else
#	pragma comment(lib, "d3dx11.lib")
#endif

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwrite.lib")

using std::vector, std::map, std::pair;