#include "Rice/Engine/Core.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/Util/Exceptions/NullPtrException.hpp"
#include <Rice/Engine/Engine.hpp>
NSP_ENGINE_BEGIN

Engine::Engine(ptr<Core> core) : core(core) {}
ptr<Engine> Engine::create(ptr<Core> core) {
    return ptr<Engine>(new Engine(core));
}
Engine::~Engine() {}

void Engine::loadScene(ptr<Scene> scene) { lock_core()->loadScene(scene); }

void Engine::setFps(dbl fps) { lock_core()->fps = fps; }

dbl Engine::getFps() { return lock_core()->fps; }

dbl Engine::getFixedDeltaTime() { return lock_core()->fixedDeltaTime; }

dbl Engine::getDeltaTime() { return lock_core()->deltaTime; }

dbl Engine::getTime() { return lock_core()->time; }

dbl Engine::getAspectRatio() {
    auto size = lock_core()->wnd->getSize();
    return (dbl)size.x / size.y;
}

ptr<Core> Engine::lock_core() {
    auto core_lock = core.lock();
    if (!core_lock)
        THROW_NULL_PTR_EXCEPTION(core_lock.get());
    return core_lock;
}

ptr<Graphics::Shader> Engine::getOrCreateShader(
    String name, std::function<void(ptr<Graphics::Shader>)> shader_creator) {
    auto it = shaders.find(name); // try to find shader in map
    if (it == shaders.end()) {
        ptr<Graphics::Shader> shader{
            new Graphics::Shader(getGraphicsManager())}; // create shader
        shader_creator(shader); // call creator function to setup shader
        shaders[name] = shader; // add to map
        return shader;
    }
    return it->second;
}

ptr<Graphics::Material> Engine::getOrCreateMaterial(
    String name,
    std::function<ptr<Graphics::Material>(ptr<Engine>)> material_factory) {
    auto core_lock = lock_core();
    auto it = materials.find(name); // try to find material in map
    if (it == materials.end()) {
        return materials[name] = material_factory(shared_from_this()); // add to map
    }
    return it->second;
}

ptr<Graphics::GraphicsManager> Engine::getGraphicsManager() {
    return lock_core()->graphics_manager;
}

ptr<InputManager> Engine::getInputManager() {
    return lock_core()->wnd->getInputManager();
}

NSP_ENGINE_END