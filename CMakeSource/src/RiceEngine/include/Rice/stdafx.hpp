#pragma once

#define USE_BETTERCPP_MACROS
#include <BetterCpp/BetterCpp.hpp>

#include <mutex>
#include <shared_mutex>
#include <filesystem>

#include <fmt/format.h>
#include <fmt/xchar.h>

#include <magic_enum/magic_enum.hpp>

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "namespaces.h"
#include "macros.h"

NSP_ENGINE_BEGIN

using namespace bettercpp;

NSP_ENGINE_END

#include "defines.h"

#include "Math.hpp"
#include "Util.hpp"
