#pragma once
#ifndef RICE_SOURCE
#include "Rice/namespaces.h"
#ifdef RICE_SERVER
#include "ServerScene.hpp"
#else
#include "ClientScene.hpp"
#endif

NSP_ENGINE_BEGIN
#ifdef RICE_SERVER
using Scene = ServerScene;
#else
using Scene = ClientScene;
#endif
NSP_ENGINE_END
#endif // RICE_SOURCE

