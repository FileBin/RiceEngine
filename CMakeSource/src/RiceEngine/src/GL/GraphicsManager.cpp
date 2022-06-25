#include "pch.h"
#include <Rice/GL/GraphicsManager.hpp>
#include <Rice/GL/Shader.hpp>

#include <Rice/GL/CommandBuffer.hpp>

#include "Vulkan_API_code/api_GraphicsManager_impl.hpp"
#include "Vulkan_API_code/api_CommandBuffer.hpp"

NSP_GL_BEGIN

void GraphicsManager::init(pWindow _window) {
	window = _window;
	api_data = new_ref<GraphicsManager_API_data>(refptr_this());
	Log::debug("GraphicsManager initializing...");

	is_initialized = true;
	Log::debug("Successfully initialized!");

	_window->resize_event.subscribe(resizeReg, // @suppress("Invalid arguments")
	[this](pWindow win) {
		api_data->recreateSwapchain();
		api_data->resizing = false;
	});

}

void GraphicsManager::executeCmd(pCommandBuffer cmd) {
	api_data->executeCmd({ cmd->api_data->cmd[api_data->swapchainImageIndex] });
}

void GraphicsManager::executeCmds(vec<pCommandBuffer> cmds) {
	uint n = cmds.size();
	vec<vk::CommandBuffer> c(n);
	for (uint i = 0; i < n; ++i)
		c[i] = cmds[i]->api_data->cmd[api_data->swapchainImageIndex];

	api_data->executeCmd(c);
}


//FINALIZER
void GraphicsManager::cleanup() {
	if(is_initialized) {
		Log::debug("Graphics manager cleanup...");
		api_data.release();
	}
	is_initialized = false;
}

NSP_GL_END
