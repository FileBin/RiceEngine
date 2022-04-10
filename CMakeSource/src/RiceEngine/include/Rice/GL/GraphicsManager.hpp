#include "../stdafx.hpp"
NSP_GL_BEGIN
class GraphicsManager;
typedef SmartPtr<GraphicsManager> pGraphicsManager;
NSP_GL_END
#pragma once

#include "VulkanHelper.hpp"
#include "../Engine/Window.hpp"
#include "GraphicsComponentBase.hpp"


NSP_GL_BEGIN

class GraphicsManager : public ICleanable {
private:
	friend class GraphicsComponentBase;
	bool is_initialized = false;

	vk::Instance vk_instance; // Vulkan library handle
	vk::DebugUtilsMessengerEXT vk_debug_messenger;// Vulkan debug output handle
	vk::PhysicalDevice vk_GPU;// GPU chosen as the default device
	vk::Device vk_device;// Vulkan device for commands
	vk::SurfaceKHR vk_surface;// Vulkan window surface

	vk::SwapchainKHR vk_swapchain = nullptr;
	vk::Format vk_swapchainImageFormat = vk::Format::eUndefined;// image format expected by the windowing system

	vec<vk::Image> vk_swapchainImages = {};//array of images from the swapchain
	vec<vk::ImageView> vk_swapchainImageViews = {};//array of image-views from the swapchain
	vec<vk::Framebuffer> vk_framebuffers = {};

	vk::Queue vk_graphicsQueue;//queue we will submit to
	uint vk_graphicsQueueFamily = 0;//family of that queue

	vk::CommandPool vk_commandPool;//the command pool for our commands
	vk::CommandBuffer vk_mainCommandBuffer;//the buffer we will record into

	vk::RenderPass vk_def_renderPass;

	vk::Semaphore vk_presentSemaphore, vk_renderSemaphore;
	vk::Fence vk_renderFence;

	uint frameNumber = 0;
	uint swapchainImageIndex = 0;

	pWindow window;

	Window::ResizeEvent::UUID on_resize_uuid;

	bool isDrawing = false;

    bool use_hdr =
#ifdef USE_HDR
    		true;
#else
    		false;
#endif
public:
    Window::ResizeEvent resizeEvent;

	GraphicsManager() = default;
	~GraphicsManager() override { cleanup(); }

	pWindow getWindow() { return window; }

	void init(pWindow window);

	bool beginDraw();
	void draw(uint count);
	void endDraw();
	void cleanup() override;

private:
	void init_swapchain();
	void init_commands();
	void init_def_renderpass();
	void init_framebuffers();
	void init_sync_structures();

	void register_events();


	void onResize(pWindow sender);
	void cleanupSwapChain();
	void recreateSwapChain();
};

NSP_GL_END
