#include "Rice/Engine/Log.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/namespaces.h"
#include "pch.h"

#include <Rice/GL/Material.hpp>

NSP_GL_BEGIN

Material::Material(ptr<GraphicsManager> manager, ptr<Shader> shader)
    : graphics_manager(manager), shader(shader) {}

void Material::updateBuffer() {
    //TODO: implement updateBuffer function for Material class
    Log::log(Log::Warning, "Material::updateBuffer() not implemented yet");
}

NSP_GL_END