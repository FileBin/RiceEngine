#include "pch.h"
#include <Rice/GL/GraphicsManager.hpp>
#include <Rice/version.h>
#include <Rice/Engine/Log.hpp>

using namespace ::Rice::Graphics;
NSP_GL_BEGIN

void GraphicsManager::init(pWindow _window) {
	Log::debug("GraphicsManager initializing...");
	window = _window;

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
	vk_instance = vkb_inst.instance;

	//store the debug messenger
#ifdef _DEBUG
	vk_debug_messenger = vkb_inst.debug_messenger;
#endif

	SDL_Vulkan_CreateSurface(window->getHandle().get(), vk_instance, (VkSurfaceKHR*)&vk_surface);

	//use vkbootstrap to select a GPU.
	//We want a GPU that can write to the SDL surface and supports Vulkan 1.1
	vkb::PhysicalDeviceSelector selector {vkb_inst};
	vkb::PhysicalDevice physicalDevice = selector
	.set_minimum_version(1, 1)
	.set_surface(vk_surface)
	.prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
	.select()
	.value();

	//create the final Vulkan device
	vkb::DeviceBuilder deviceBuilder {physicalDevice};

	vkb::Device vkbDevice = deviceBuilder.build().value();

	// Get the VkDevice handle used in the rest of a Vulkan application
	vk_device = vkbDevice.device;
	vk_GPU = physicalDevice.physical_device;

	init_swapchain();

	Log::debug("Successfully initialized!");
}

void GraphicsManager::init_swapchain() {
	Log::debug("Swapchain building...");
	vkb::SwapchainBuilder swapchainBuilder {vk_GPU,vk_device, vk_surface};

	if(use_hdr) {
		swapchainBuilder.set_desired_format(
				(VkSurfaceFormatKHR)vk::SurfaceFormatKHR(
						vk::Format::eR16G16B16A16Sfloat,
						vk::ColorSpaceKHR::eHdr10HlgEXT));
	} else {
		swapchainBuilder.use_default_format_selection();
	}

	vkb::Swapchain vkbSwapchain =
	swapchainBuilder
	//use v-sync present mode
	.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
	.set_desired_extent(window->getWidth(), window->getHeight())
	.build()
	.value();

	//store swapchain and its related images
	vk_swapchain = vkbSwapchain.swapchain;
	vk_swapchainImages = (vec<vk::Image>&)vkbSwapchain.get_images().value();
	vk_swapchainImageViews = (vec<vk::ImageView>&)vkbSwapchain.get_image_views().value();

	vk_swapchainImageFormat = (vk::Format&)vkbSwapchain.image_format;
	Log::debug("Swapchain built!");
}

void GraphicsManager::cleanup() {
	Log::debug("Graphics manager cleanup...");
}

NSP_GL_END
