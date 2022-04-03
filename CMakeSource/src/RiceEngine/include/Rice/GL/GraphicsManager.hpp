#include "../stdafx.hpp"
NSP_GL_BEGIN
class GraphicsManager;
typedef SmartPtr<GraphicsManager> pGraphicsManager;
NSP_GL_END
#pragma once

#include "VulkanHelper.hpp"
#include "../Engine/Window.hpp"


NSP_GL_BEGIN

class GraphicsManager : public ICleanable {
private:
	bool is_initialized = false;

	vk::Instance vk_instance = nullptr; // Vulkan library handle
	vk::DebugUtilsMessengerEXT vk_debug_messenger;// Vulkan debug output handle
	vk::PhysicalDevice vk_GPU = nullptr;// GPU chosen as the default device
	vk::Device vk_device = nullptr;// Vulkan device for commands
	vk::SurfaceKHR vk_surface = nullptr;// Vulkan window surface

	vk::SwapchainKHR vk_swapchain = nullptr;
	vk::Format vk_swapchainImageFormat = vk::Format::eUndefined;// image format expected by the windowing system
	vec<vk::Image> vk_swapchainImages = {};//array of images from the swapchain
	vec<vk::ImageView> vk_swapchainImageViews = {};//array of image-views from the swapchain

	vk::Queue vk_graphicsQueue = nullptr;//queue we will submit to
	uint vk_graphicsQueueFamily = 0;//family of that queue

	vk::CommandPool vk_commandPool = nullptr;//the command pool for our commands
	vk::CommandBuffer vk_mainCommandBuffer = nullptr;//the buffer we will record into
    pWindow window;

    bool use_hdr =
#ifdef USE_HDR
    		true;
#else
    		false;
#endif
public:
	GraphicsManager() = default;
	~GraphicsManager() override { cleanup(); }

	void init(pWindow window);
	void init_swapchain();
	void init_commands();

	void cleanup() override;
};

NSP_GL_END
