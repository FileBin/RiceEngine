#ifndef RICE_SOURCE
#include "Rice/namespaces.h"
#ifdef RICE_SERVER
#include "Rice/Engine/ServerCore.hpp"
#else
#include "Rice/Engine/ClientCore.hpp"
#endif

NSP_ENGINE_BEGIN
#ifdef RICE_SERVER
using Core = ServerCore;
#else
using Core = ClientCore;
#endif
NSP_ENGINE_END
#endif // RICE_SOURCE