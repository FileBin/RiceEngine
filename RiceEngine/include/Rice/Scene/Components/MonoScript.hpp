#include "Rice/stdafx.hpp"

NSP_COMPONENTS_BEGIN
template <typename T>
class MonoScript;
NSP_COMPONENTS_END

#pragma once
#include "Rice/Scene/PackableComponentT.hpp"

#include "Rice/Scene/SceneBase.hpp"
#include "Rice/Scene/SceneRender.hpp"
#include "Rice/Engine/EngineBase.hpp"

NSP_COMPONENTS_BEGIN
    template<typename T>
	class MonoScript : public virtual PackableComponentT<T> {
	};
NSP_COMPONENTS_END