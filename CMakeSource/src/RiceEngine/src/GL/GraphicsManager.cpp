#include "pch.h"
#include <Rice/GL/GraphicsManager.hpp>
#include <Rice/GL/Shader.hpp>

#include <Rice/GL/CommandBuffer.hpp>

#include "Vulkan_API_code/api_GraphicsManager_impl.hpp"
#include "Vulkan_API_code/api_CommandBuffer.hpp"

NSP_GL_BEGIN

void GraphicsManager::init(pWindow _window) {
	Log::debug("GraphicsManager initializing...");
	window = _window;
	api_data = new GraphicsManager_API_data(this);

	is_initialized = true;
	Log::debug("Successfully initialized!");

	_window->resize_event.subscribe([this](pWindow win) { // @suppress("Invalid arguments")
		api_data->recreateSwapchain();
		api_data->resizing = false;
	});

}

void GraphicsManager::drawCmd(pCommandBuffer cmd) {
	api_data->drawCmd(cmd->api_data->cmd[api_data->swapchainImageIndex]);
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
