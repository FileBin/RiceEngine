#include "../stdafx.hpp"
#include "BetterCpp/Objects/BasePtr.hpp"
#include "BetterCpp/Objects/PtrTypes.hpp"
#include "Rice/macros.h"
#include <functional>

NSP_ENGINE_BEGIN

class PTR_PROTO(Engine);

NSP_ENGINE_END

#pragma once
#include "Core.hpp"
#include "../GL/GraphicsManager.hpp"
#include "../GL/Shader.hpp"
#include "../GL/Material.hpp"
//#include "../AL/SoundManager.hpp" //TODO

#include "../Scene/Scene.hpp"

NSP_ENGINE_BEGIN

class Engine : public EnableThisRefPtr<Engine> {
public:
	Engine(pCore core);
	~Engine();

	void loadScene(pScene scene);

	void setFps(dbl fps = 60.);
    dbl getFps();
    
	dbl getFixedDeltaTime();
	dbl getDeltaTime();
	dbl getTime();

	Graphics::pShader getOrCreateShader(String name, std::function<void(Graphics::pShader)> shader_creator);
	Graphics::pMaterial getOrCreateMaterial(String name, std::function<Graphics::pShader(pEngine)> shader_factory);
	Graphics::pGraphicsManager getGraphicsManager();

private:
	pCore core;
    map<String, Graphics::pShader> shaders;
    map<String, Graphics::pMaterial> materials;
};

NSP_ENGINE_END
