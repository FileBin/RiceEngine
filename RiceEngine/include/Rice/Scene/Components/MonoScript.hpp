#include "Rice/stdafx.hpp"

NSP_COMPONENTS_BEGIN
template <typename T>
class MonoScript;
NSP_COMPONENTS_END

#pragma once
#include "Rice/Scene/PackableComponentT.hpp"

#include "Rice/Scene/Scene.hpp"
#include "Rice/Scene/SceneRender.hpp"
#include "Rice/Engine/Engine.hpp"

NSP_COMPONENTS_BEGIN
    template<typename T>
	class MonoScript : public virtual PackableComponentT<T> {
	};
NSP_COMPONENTS_END