#include "pch.h"
#include <Rice/GL/GraphicsManager.hpp>
#include <Rice/version.h>
#include <Rice/Engine/Log.hpp>
#include <Rice/GL/VulkanHelper.hpp>

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

	// use vkbootstrap to get a Graphics queue
	vk_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	vk_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	init_swapchain();
	init_commands();

	is_initialized = true;

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

	vkb::Swapchain vkbSwapchain = swapchainBuilder
	.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) //use v-sync present mode
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

void GraphicsManager::init_commands() {
	//create a command pool for commands submitted to the graphics queue.
	//we also want the pool to allow for resetting of individual command buffers
	auto commandPoolInfo = VulkanHelper::command_pool_create_info(vk_graphicsQueueFamily, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

	vk::Result res = (vk::Result)vkCreateCommandPool(vk_device, (VkCommandPoolCreateInfo*)&commandPoolInfo, nullptr, (VkCommandPool*)&vk_commandPool);
	//TODO make error handling

	//allocate the default command buffer that we will use for rendering
	auto cmdAllocInfo = VulkanHelper::command_buffer_allocate_info(vk_commandPool);

	res = (vk::Result)vkAllocateCommandBuffers(vk_device, (VkCommandBufferAllocateInfo*)&cmdAllocInfo, (VkCommandBuffer*)&vk_mainCommandBuffer);
	//TODO make error handling
}


//FINALIZER
void GraphicsManager::cleanup() {
	if(is_initialized) {
		Log::debug("Graphics manager cleanup...");

		vkDestroyCommandPool(vk_device, vk_commandPool, nullptr);

		vkDestroySwapchainKHR(vk_device, vk_swapchain, nullptr);
		vk_swapchain = nullptr;

		//destroy swapchain resources
		for (int i = 0; i < vk_swapchainImageViews.size(); i++) {
			vkDestroyImageView(vk_device, vk_swapchainImageViews[i], nullptr);
		}
		vk_swapchainImageViews.clear();
		vk_swapchainImages.clear();

		vkDestroyDevice(vk_device, nullptr);
		vk_device = nullptr;

		vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);
		vk_surface = nullptr;

		vkb::destroy_debug_utils_messenger(vk_instance, vk_debug_messenger);
		vk_debug_messenger = nullptr;

		vkDestroyInstance(vk_instance, nullptr);
		vk_instance = nullptr;
	}
	is_initialized = false;
}

NSP_GL_END
