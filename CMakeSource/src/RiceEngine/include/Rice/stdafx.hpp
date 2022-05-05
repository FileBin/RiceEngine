#pragma once

#include <codecvt>
#include <locale>

#include <iomanip>
#include <clocale>
#include <ctime>
#include <time.h>
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
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <functional>
#include <thread>

#include <fmt/format.h>
#include <fmt/xchar.h>

#include <magic_enum/magic_enum.hpp>

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#define USE_BETTERCPP_MACROS
#include <BetterCpp/BetterCpp.hpp>

#include "namespaces.h"
#include "macros.h"

#include "defines.h"

NSP_ENGINE_BEGIN

using namespace bettercpp;

NSP_ENGINE_END

