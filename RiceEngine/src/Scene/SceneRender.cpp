#include "Rice/Engine/InputManager.hpp"
#include "Rice/Math/Vectors/Vector2.hpp"
#include "Rice/Scene/Components/Camera.hpp"
#include <Rice/GL/RenderingMesh.hpp>
#include <Rice/Scene/SceneRender.hpp>

NSP_ENGINE_BEGIN

SceneRender::SceneRender(ptr<Scene> scene, ptr<Engine> engine)
    : scene(scene), engine(engine) {}

void SceneRender::registerMesh(ptr<Graphics::RenderingMesh> mesh) {
    mesh->Register(mesh_collection.registerPtr(mesh));
}

void SceneRender::unregisterMesh(ptr<Graphics::RenderingMesh> mesh) {
    mesh_collection.unregister(mesh->Unregister());
}

void SceneRender::draw(ptr<Components::Camera> camera) {
    uint count = update(camera);
    auto g_mgr = engine->getGraphicsManager();
    g_mgr->executeCmds(getCmds(count));
    // TODO add transparent queue
}

uint SceneRender::update(ptr<Components::Camera> camera) {
    // TODO remove this
    // Vector2 mouse_pos = InputManager::getMousePos();
    // mouse_pos *= 2;
    // mouse_pos -= Vector2(0.5, 0.5);
    // Log::log(Log::Info, "Mouse pos: {} {}", mouse_pos.x, mouse_pos.y);
    // camera->getTransform()->setRotation(Quaternion::fromEulerAngles(mouse_pos.y,
    // mouse_pos.x, 0));
    uint count = 0;
    auto coll = mesh_collection.getCollectionWithGaps();
    for (auto ptr : coll) {
        if (ptr) {
            auto proj = camera->getProjectionMatrix();
            ptr->updateConstBuffer(camera->getViewMatrix(),
                                   proj);
            ptr->updateCmdBuffer();
            count++;
        }
    }
    return count;
}

vec<ptr<Graphics::CommandBuffer>> SceneRender::getCmds(uint count) {
    vec<ptr<Graphics::CommandBuffer>> output(count);
    uint i = 0;
    auto coll = mesh_collection.getCollectionWithGaps();
    for (auto ptr : coll) {
        if (ptr) {
            output[i++] = ptr->getCmd();
        }
    }
    return output;
}

void SceneRender::cleanup() {
    mesh_collection.cleanup();
}

NSP_ENGINE_END