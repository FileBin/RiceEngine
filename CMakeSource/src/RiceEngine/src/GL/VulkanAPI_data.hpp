/*
 * VulkanAPI_data.hpp
 *
 *  Created on: 26 апр. 2022 г.
 *      Author: FileBinsLapTop
 */

#pragma once

#include "VulkanHelper.hpp"

struct API_Data {
	vk::Instance instance; // Vulkan library handle
	vk::DebugUtilsMessengerEXT debug_messenger;// Vulkan debug output handle
	vk::PhysicalDevice GPU;// GPU chosen as the default device
	vk::Device device;// Vulkan device for commands
	vk::SurfaceKHR surface;// Vulkan window surface

	vk::SwapchainKHR swapchain = nullptr;
	vk::Format swapchainImageFormat = vk::Format::eUndefined;// image format expected by the windowing system

	vec<vk::Image> swapchainImages = {};//array of images from the swapchain
	vec<vk::ImageView> swapchainImageViews = {};//array of image-views from the swapchain
	vec<vk::Framebuffer> framebuffers = {};

	vk::Queue graphicsQueue;//queue we will submit to
	uint graphicsQueueFamily = 0;//family of that queue

	vk::CommandPool commandPool;//the command pool for our commands
	vk::CommandBuffer mainCommandBuffer;//the buffer we will record into

	vk::RenderPass def_renderPass;

	vk::Semaphore presentSemaphore, renderSemaphore;
	vk::Fence renderFence;

	vk::Extent2D windowExcent;
};

