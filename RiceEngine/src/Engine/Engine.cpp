#include "pch.h"

#include "Rice/Engine/Core.hpp"
#include <Rice/Engine/Engine.hpp>
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Shader.hpp"
NSP_ENGINE_BEGIN

Engine::Engine(pCore core) : core(core) {}
Engine::~Engine() {}

void Engine::loadScene(pScene scene) { core->loadScene(scene); }

void Engine::setFps(dbl fps) { core->fps = fps; }

dbl Engine::getFps() { return core->fps; }

dbl Engine::getFixedDeltaTime() { return core->fixedDeltaTime; }

dbl Engine::getDeltaTime() { return core->deltaTime; }

dbl Engine::getTime() { return core->time; }

Graphics::pShader Engine::getOrCreateShader(
    String name, std::function<void(Graphics::pShader)> shader_creator) {
    auto it = shaders.find(name); // try to find shader in map
    if (it == shaders.end()) {
        auto shader =
            new_ref<Graphics::Shader>(getGraphicsManager()); // create shader
        shader_creator(shader); // call creator function to setup shader
        shaders[name] = shader; // add to map
        return shader;
    }
    return it->second;
}

Graphics::pMaterial Engine::getOrCreateMaterial(
    String name, std::function<Graphics::pShader(pEngine)> shader_factory) {
    auto it = materials.find(name); // try to find material in map
    if (it == materials.end()) {
        auto material = new_ref<Graphics::Material>(
            core->graphics_manager,
            shader_factory(refptr_this())); // create material
        materials[name] = material;         // add to map
        return material;
    }
    return it->second;
}

Graphics::pGraphicsManager Engine::getGraphicsManager() {
    return core->graphics_manager;
}

NSP_ENGINE_END