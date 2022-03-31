#include "../stdafx.hpp"
NSP_GL_BEGIN
class GraphicsManager;
NSP_GL_END
#pragma once

NSP_GL_BEGIN

class GraphicsManager {
private:

    vk::Instance vkInstance; // Vulkan library handle
    vk::DebugUtilsMessengerEXT vkDebugMessenger; // Vulkan debug output handle
    vk::PhysicalDevice vkGPU; // GPU chosen as the default device
    vk::Device vkDevice; // Vulkan device for commands
    vk::SurfaceKHR _surface; // Vulkan window surface
public:
	GraphicsManager() = default;
	~GraphicsManager() = default;

	void init(uint videoCardIdx = 0);
private:
    void initInstance();


};

NSP_GL_END
