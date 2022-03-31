#pragma once

#include <clocale>
#include <ctime>
#include <stdexcept>
#include <fstream>
#include <filesystem>

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

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <VkBootstrap.h>

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
