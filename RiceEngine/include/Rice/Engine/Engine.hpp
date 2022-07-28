#include "../stdafx.hpp"
#include "Rice/defines.h"

NSP_ENGINE_BEGIN
class Engine;
NSP_ENGINE_END

#pragma once
#include "Core.hpp"
#include "../GL/GraphicsManager.hpp"
#include "../GL/Shader.hpp"
#include "../GL/Material.hpp"
//#include "../AL/SoundManager.hpp" //TODO

#include "../Scene/Scene.hpp"

NSP_ENGINE_BEGIN

class Engine : public enable_ptr<Engine> {
public:
	Engine(ptr<Core> core);
	~Engine();

	void loadScene(ptr<Scene> scene);

	void setFps(dbl fps = 60.);
    dbl getFps();
    
	dbl getFixedDeltaTime();
	dbl getDeltaTime();
	dbl getTime();

 	ptr<Graphics::Shader> getOrCreateShader(String name, std::function<void(Graphics::pShader)> shader_creator);
	ptr<Graphics::Material> getOrCreateMaterial(String name, std::function<Graphics::pShader(pEngine)> shader_factory);
	ptr<Graphics::GraphicsManager> getGraphicsManager();

private:
	ptr<Core> core;
    umap<String, ptr<Graphics::Shader>> shaders;
    umap<String, ptr<Graphics::Material>> materials;
};

NSP_ENGINE_END
