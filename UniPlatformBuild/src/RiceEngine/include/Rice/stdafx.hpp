#pragma once

#include <clocale>
#include <ctime>
#include <stdexcept>
#include <fstream>

#include <typeinfo>
#include <typeindex>

#include <mutex>
#include <shared_mutex>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <functional>

#include <fmt/format.h>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif //_WIN32

#ifdef _MSC_VER

#include "Dbghelp.h"
#pragma comment(lib, "Dbghelp.lib")

#endif

#include "defines.h"
#include "namespaces.h"
#include "Util/SmartPointer.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#pragma comment(lib, "GameEngine.lib")

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "ogg.lib")
#pragma comment(lib, "libvorbis_static.lib")
#pragma comment(lib, "libvorbisfile_static.lib")
#pragma comment(lib, "vorbisenc.lib")