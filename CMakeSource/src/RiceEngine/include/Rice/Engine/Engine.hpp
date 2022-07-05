#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class Engine;
typedef RefPtr<Engine> pEngine;

NSP_ENGINE_END

#pragma once
#include "Core.hpp"
#include "../GL/GraphicsManager.hpp"
//#include "../AL/SoundManager.hpp" //TODO

#include "../Scene/Scene.hpp"

NSP_ENGINE_BEGIN

class Engine {
public:
	Engine(pCore Core);
	~Engine();

	uint getMsaaLevel() const {
		return msaaLevel;
	}

	void setMsaaLevel(uint level);

	void setLoadingScene(Scening::pScene scene);

	void loadScene(Scening::pScene scene);

	void setFps(float fps = 60.f);

	void postUpdate();

	double getFixedDeltaTime();
	double getDeltaTime();
	double getTime();

	void registerScript(pScriptBase script, Stage stage = Stage::Update);

	pMaterial createMaterial(pShader shader);
	pShader createShader();

	Graphics::pGraphicsManager getGraphicsManager();
    //TODO Audio::pSoundManader getSoundManager();

private:
	pCore core;
	uint msaaLevel = 1;
};

NSP_ENGINE_END
