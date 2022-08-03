#include "../stdafx.hpp"
#include "Rice/Engine/InputManager.hpp"

NSP_ENGINE_BEGIN
class Engine;
NSP_ENGINE_END

#pragma once
#include "Core.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Shader.hpp"
//#include "../AL/SoundManager.hpp" //TODO

#include "../Scene/Scene.hpp"

NSP_ENGINE_BEGIN

class Engine : public enable_ptr<Engine> {
  private:
    Engine(ptr<Core> core);

  public:
    static ptr<Engine> create(ptr<Core> core);
    ~Engine();

    void loadScene(ptr<Scene> scene);

    void setFps(dbl fps = 60.);
    dbl getFps();

    dbl getFixedDeltaTime();
    dbl getDeltaTime();
    dbl getTime();

    ptr<Graphics::Shader> getOrCreateShader(
        String name, std::function<void(ptr<Graphics::Shader>)> shader_creator);
    ptr<Graphics::Material> getOrCreateMaterial(
        String name,
        std::function<ptr<Graphics::Shader>(ptr<Engine>)> shader_factory);
    ptr<Graphics::GraphicsManager> getGraphicsManager();
    ptr<InputManager> getInputManager();

  private:
    ptr<Core> lock_core();

    wptr<Core> core;
    map<String, ptr<Graphics::Shader>> shaders;
    map<String, ptr<Graphics::Material>> materials;
};

NSP_ENGINE_END
