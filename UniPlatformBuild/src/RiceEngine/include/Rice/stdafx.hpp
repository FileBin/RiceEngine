#pragma once

#ifndef _MSC_VER
#include "mingw_defines.h"
#endif

#include <clocale>
#include <ctime>
#include <stdexcept>
#include <fstream>
#include <shared_mutex>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <functional>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif //_WIN32

#include "namespaces.h"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#pragma comment(lib, "GameEngine.lib")

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "ogg.lib")
#pragma comment(lib, "libvorbis_static.lib")
#pragma comment(lib, "libvorbisfile_static.lib")
#pragma comment(lib, "vorbisenc.lib")