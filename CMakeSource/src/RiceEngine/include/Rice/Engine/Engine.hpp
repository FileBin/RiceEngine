#include "../stdafx.hpp"
#include "BetterCpp/Objects/PtrTypes.hpp"
#include "Rice/macros.h"

NSP_ENGINE_BEGIN

class PTR_PROTO(Engine);

NSP_ENGINE_END

#pragma once
#include "Core.hpp"
#include "../GL/GraphicsManager.hpp"
#include "../GL/Shader.hpp"
//#include "../AL/SoundManager.hpp" //TODO

#include "../Scene/Scene.hpp"

NSP_ENGINE_BEGIN

class Engine {
public:
	Engine(pCore& core);
	~Engine();

	void setLoadingScene(pScene scene);

	void loadScene(pScene scene);

	void setFps(dbl fps = 60.);

	void postUpdate();

	dbl getFixedDeltaTime();
	dbl getDeltaTime();
	dbl getTime();

	Graphics::pShader createShader();
	Graphics::pGraphicsManager getGraphicsManager();

private:
	pCore& core;
};

NSP_ENGINE_END
