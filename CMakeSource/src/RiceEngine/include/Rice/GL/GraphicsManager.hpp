#include "../stdafx.hpp"
NSP_GL_BEGIN
class GraphicsManager;
typedef SmartPtr<GraphicsManager> pGraphicsManager;
NSP_GL_END
#pragma once

#include "../Engine/Window.hpp"

NSP_GL_BEGIN

class GraphicsManager : public ICleanable {
private:
    vk::Instance vk_instance; // Vulkan library handle
    vk::DebugUtilsMessengerEXT vk_debug_messenger; // Vulkan debug output handle
    vk::PhysicalDevice vk_GPU; // GPU chosen as the default device
    vk::Device vk_device; // Vulkan device for commands
    vk::SurfaceKHR vk_surface; // Vulkan window surface

	// --- other code ---
	vk::SwapchainKHR vk_swapchain; // from other articles

	// image format expected by the windowing system
	vk::Format vk_swapchainImageFormat = vk::Format::eUndefined;

	//array of images from the swapchain
	vec<vk::Image> vk_swapchainImages;

	//array of image-views from the swapchain
	vec<vk::ImageView> vk_swapchainImageViews;

    pWindow window;

    bool use_hdr =
#ifdef USE_HDR
    		true;
#else
    		false;
#endif
public:
	GraphicsManager() = default;
	~GraphicsManager() override {
		cleanup();
	}

	void init(pWindow window);
	void init_swapchain();

	void cleanup() override;
};

NSP_GL_END
