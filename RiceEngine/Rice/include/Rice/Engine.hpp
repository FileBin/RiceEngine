
#include "Rice/Scene/Component.hpp"
#include "Rice/Scene/Object.hpp"
#include "Rice/Scene/SceneBase.hpp"
#ifndef RICE_SOURCE
#include "Rice/Engine/Core.hpp"
#include "Rice/Scene/Components/all.h"
#include "Rice/Scene/Scene.hpp"
#include "Rice/namespaces.h"
#ifdef RICE_SERVER
#include "Rice/Engine/ServerEngine.hpp"
#else
#include "Rice/Engine/ClientEngine.hpp"
#endif

NSP_ENGINE_BEGIN
#ifdef RICE_SERVER
using Engine = ServerEngine;
#else
using Engine = ClientEngine;
#endif
NSP_ENGINE_END
#endif // RICE_SOURCE