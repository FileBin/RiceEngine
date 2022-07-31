﻿#include "pch.h"
#include <Rice/GL/GraphicsManager.hpp>
#include <Rice/GL/Shader.hpp>

#include <Rice/GL/CommandBuffer.hpp>

#include "Vulkan_API_code/api_CommandBuffer.hpp"
#include "Vulkan_API_code/api_GraphicsManager_impl.hpp"

NSP_GL_BEGIN

GraphicsManager::GraphicsManager() = default;

ptr<GraphicsManager> GraphicsManager::create() {
    return ptr<GraphicsManager>(new GraphicsManager());
}

void GraphicsManager::init(ptr<Window> _window) {
    window = _window;
    api_data.reset(new GraphicsManager_API_data(shared_from_this()));
    Log::debug("GraphicsManager initializing...");

    is_initialized = true;
    Log::debug("Successfully initialized!");

    _window->resize_event->subscribe(
        resizeReg, // @suppress("Invalid arguments")
        [this](ptr<Window> win) {
            api_data->recreateSwapchain();
            api_data->resizing = false;
        });
}

void GraphicsManager::executeCmd(ptr<CommandBuffer> cmd) {
    api_data->executeCmd({cmd->api_data->cmd[api_data->swapchainImageIndex]});
}

void GraphicsManager::executeCmds(vec<ptr<CommandBuffer>> cmds) {
    uint n = cmds.size();
    vec<vk::CommandBuffer> c(n);
    for (uint i = 0; i < n; ++i)
        c[i] = cmds[i]->api_data->cmd[api_data->swapchainImageIndex];

    api_data->executeCmd(c);
}

// FINALIZER
void GraphicsManager::cleanup() {
    if (is_initialized) {
        Log::debug("Graphics manager cleanup...");
        delete api_data.release();
    }
    is_initialized = false;
}

GraphicsManager::~GraphicsManager() { cleanup(); }

NSP_GL_END
