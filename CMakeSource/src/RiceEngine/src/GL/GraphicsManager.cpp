#include "pch.h"
#include <Rice/GL/GraphicsManager.hpp>
#include <Rice/version.h>

NSP_GL_BEGIN

void GraphicsManager::init(uint videoCardIdx) {
	initInstance();
}

void GraphicsManager::initInstance() {
	vkb::InstanceBuilder builder;

	//make the Vulkan instance
	auto inst_ret = builder.set_app_name("Game")
#ifdef _DEBUG
	.request_validation_layers(true)
#endif
	.require_api_version(1, 1, 0)
#ifdef _DEBUG
	.use_default_debug_messenger()
#endif
	.build();

	vkb::Instance vkb_inst = inst_ret.value();

	//store the instance
	vkInstance = vkb_inst.instance;

	//store the debug messenger
#ifdef _DEBUG
	vkDebugMessenger = vkb_inst.debug_messenger;
#endif
}

NSP_GL_END
