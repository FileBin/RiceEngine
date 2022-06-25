/*
 * VulkanAPI_data_impl.hpp
 *
 *  Created on: 26 April 2022
 *      Author: FileBinsLapTop
 */

#pragma once

#include "api_GraphicsManager.hpp"

#include <Rice/GL/GraphicsManager.hpp>
#include <Rice/Engine/Log.hpp>

#include <Rice/Math.hpp>

#include <Rice/Engine/Window.hpp>

NSP_GL_BEGIN

GraphicsManager_API_data::GraphicsManager_API_data(pGraphicsManager mgr) {
	vkb::InstanceBuilder builder;

	g_mgr = mgr;

	//make the Vulkan instance
	auto inst_ret = builder.set_app_name("Riced Field")
	.request_validation_layers(true)
	.require_api_version(1, 1, 0)
	.use_default_debug_messenger()
	.build();

	if(!inst_ret.has_value())
		THROW_VK_EX(inst_ret.vk_result());

	vkb::Instance vkb_inst = inst_ret.value();

	//store the instance
	instance = vkb_inst.instance;

	//store the debug messenger
#ifdef _DEBUG
	debug_messenger = vkb_inst.debug_messenger;
#endif

	if(!SDL_Vulkan_CreateSurface(mgr->window->getHandle().get(), instance, (VkSurfaceKHR*)&surface)){ // @suppress("Invalid arguments") // @suppress("Method cannot be resolved")
		THROW_EXCEPTION("Failed to create vulkan surface! (Maybe window is not created)");
	}

	if(!surface) THROW_NULL_PTR_EXCEPTION(nullptr);

	//use vkbootstrap to select a GPU.
	//We want a GPU that can write to the SDL surface and supports Vulkan 1.1
	vkb::PhysicalDeviceSelector selector {vkb_inst};

	auto dev_result = selector
			.set_minimum_version(1, 1)
			.set_surface(surface)
#ifdef WIN32
			.prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
#endif
			.select();

	if(!dev_result.has_value()) {
		std::string str = fmt::format("Error: {}!", dev_result.error().message()); // @suppress("Invalid arguments")
		THROW_EXCEPTION(str.c_str());
	}

	vkb::PhysicalDevice physicalDevice =
			dev_result.value();


	//create the final Vulkan device
	vkb::DeviceBuilder deviceBuilder {physicalDevice};

	vkb::Device vkbDevice = deviceBuilder.build().value();

	// Get the VkDevice handle used in the rest of a Vulkan application
	device = vkbDevice.device;

	GPU = physicalDevice.physical_device;
	Log::log(Log::Info, "GPU INFO: {}", String(GPU.getProperties().deviceName.data()));

	// use vkbootstrap to get a Graphics queue
	graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	init_swapchain();
	init_commands();
	init_def_renderpass();
	init_framebuffers();
	init_sync_structures();
}

void GraphicsManager_API_data::init_swapchain() {
	Log::debug("Swapchain building...");
	vkb::SwapchainBuilder swapchainBuilder {GPU, device, surface};

	if(use_hdr) {
		swapchainBuilder.set_desired_format(
				(VkSurfaceFormatKHR)vk::SurfaceFormatKHR(
						vk::Format::eR16G16B16A16Sfloat,
						vk::ColorSpaceKHR::eHdr10HlgEXT));
	} else {
		swapchainBuilder.use_default_format_selection();
	}

	uint w,h;
	w = g_mgr->window->getWidth();
	h = g_mgr->window->getHeight();

	if(w >= 64 && h >= 64) {
		for (size_t i = 0; i < swapchainImageViews.size(); i++) {
			device.destroy(swapchainImageViews[i]);
		}

		windowExcent.width = w;
		windowExcent.height = h;
		vkb::Swapchain vkbSwapchain = swapchainBuilder
		.set_desired_present_mode((VkPresentModeKHR)vk::PresentModeKHR::eFifo) //use v-sync present mode
		//.set_desired_extent(windowExcent.width, windowExcent.height)
		.set_old_swapchain((VkSwapchainKHR)swapchain)
		.build()
		.value();

		//store swapchain and its related images
		swapchain = vkbSwapchain.swapchain;
		swapchainImages = (vec<vk::Image>&)vkbSwapchain.get_images().value();
		swapchainImageViews = (vec<vk::ImageView>&)vkbSwapchain.get_image_views().value();

		swapchainImageFormat = (vk::Format&)vkbSwapchain.image_format;
	}
	Log::debug("Swapchain built!");
}

void GraphicsManager_API_data::init_commands() {
	//create a command pool for commands submitted to the graphics queue.
	//we also want the pool to allow for resetting of individual command buffers
	vk::CommandPoolCreateInfo commandPoolInfo = VulkanHelper::command_pool_create_info(graphicsQueueFamily, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

	commandPool = device.createCommandPool(commandPoolInfo);

	//allocate the default command buffer that we will use for rendering
}

void GraphicsManager_API_data::init_def_renderpass(){
	vk::AttachmentDescription color_attachment;
	//the attachment will have the format needed by the swapchain
	color_attachment.format = swapchainImageFormat;
	//1 sample, we won't be doing MSAA
	color_attachment.samples = vk::SampleCountFlagBits::e1;
	// we Clear when this attachment is loaded
	color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
	// we keep the attachment stored when the renderpass ends
	color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
	//we don't care about stencil
	color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;//vk::AttachmentLoadOp::eClear;
	color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;//vk::AttachmentStoreOp::eStore;

	//we don't know or care about the starting layout of the attachment
	color_attachment.initialLayout = vk::ImageLayout::eUndefined;

	//after the renderpass ends, the image has to be on a layout ready for display
	color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	vk::AttachmentReference color_attachment_ref = {};
	//attachment number will index into the pAttachments array in the parent renderpass itself
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;

	//we are going to create 1 subpass, which is the minimum you can do
	vk::SubpassDescription subpass = {};
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;

	vk::RenderPassCreateInfo render_pass_info = {};

	//connect the color attachment to the info
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
	//connect the subpass to the info
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;

	auto res = device.createRenderPass(&render_pass_info, nullptr, &def_renderPass);
	THROW_VK_EX_IF_BAD(res);
}

void GraphicsManager_API_data::init_framebuffers() {
	//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	vk::FramebufferCreateInfo fb_info = {};

	fb_info.renderPass = def_renderPass;
	fb_info.attachmentCount = 1;
	fb_info.width = windowExcent.width;
	fb_info.height = windowExcent.height;
	fb_info.layers = 1;

	//grab how many images we have in the swapchain
	const uint swapchain_imagecount = swapchainImages.size();
	framebuffers = vec<vk::Framebuffer>(swapchain_imagecount);

	//create framebuffers for each of the swapchain image views
	for (int i = 0; i < swapchain_imagecount; i++) {
		fb_info.pAttachments = &swapchainImageViews[i];
		framebuffers[i] = device.createFramebuffer(fb_info);
	}
}

void GraphicsManager_API_data::init_sync_structures() {
	//create synchronization structures
	vk::FenceCreateInfo fenceCreateInfo = {};

	//we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
	fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

	renderFence = device.createFence(fenceCreateInfo);

	//for the semaphores we don't need any flags
	vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.flags = vk::SemaphoreCreateFlags();

	presentSemaphore = device.createSemaphore(semaphoreCreateInfo);
	renderSemaphore = device.createSemaphore(semaphoreCreateInfo);
}

void GraphicsManager_API_data::recreateSwapchain() {
	if(g_mgr->window->isMinimized()) {
		Log::debug("Window is minimized!");
		return;
	}
	Log::debug("Swapchain recreating...");
	device.waitIdle();
	cleanupSwapchain(false);

	init_swapchain();

	Vector2i window_size;

	SDL_Vulkan_GetDrawableSize(g_mgr->window->getHandle().get(), (int*)&window_size.x, (int*)&window_size.y);

	g_mgr->resizePipelines.invoke(window_size);
	init_framebuffers();
	g_mgr->resizeCommandBuffers.invoke(window_size);
}

void GraphicsManager_API_data::cleanupSwapchain(bool destroy) {
	uint n = framebuffers.size();
	for (size_t i = 0; i < n; i++) {
		device.destroy(framebuffers[i]);
	}

	if(destroy) {
		n = swapchainImages.size();
		for (size_t i = 0; i < n; i++) {
			device.destroy(swapchainImageViews[i]);
		}

		swapchainImageViews.clear();

		device.destroy(swapchain);
	}
}

void GraphicsManager_API_data::sync() {
	//wait until the GPU has finished rendering the last frame. Timeout of 0.1 second (fast)
	auto res = device.waitForFences( {renderFence}, true, 0.1 VK_SEC);
	if (res == vk::Result::eTimeout && !g_mgr->window->isMinimized()) {
		//wait until the GPU has finished rendering the last frame. Timeout of 1 second (slow)
		res = device.waitForFences( {renderFence}, true,
				1 VK_SEC);
	}
	THROW_VK_EX_IF_BAD(res);
}

void GraphicsManager_API_data::executeCmd(vec<vk::CommandBuffer> cmd) {
	if(resizing) return;
	sync();

	device.resetFences( {renderFence});

	//request image from the swapchain, 1 second timeout
	auto res = device.acquireNextImageKHR(swapchain, 1000000000,// @suppress("Ambiguous problem")
			presentSemaphore, nullptr, &swapchainImageIndex);// @suppress("Ambiguous problem")
	if (res == vk::Result::eErrorOutOfDateKHR) {
		resizing = true;
		return;
	}
	THROW_VK_EX_IF_BAD(res);
	//prepare the submission to the queue.
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished

	vk::SubmitInfo submit = { };
	submit.sType = vk::StructureType::eSubmitInfo;
	submit.pNext = nullptr;

	vk::PipelineStageFlags waitStage =
			vk::PipelineStageFlagBits::eColorAttachmentOutput;

	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &renderSemaphore;

	submit.commandBufferCount = cmd.size();
	submit.pCommandBuffers = cmd.data();

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	auto result = vkQueueSubmit((VkQueue) graphicsQueue, 1,
			(VkSubmitInfo*) &submit, (VkFence) renderFence);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		resizing = true;
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		THROW_VK_EX(result);
	}
	// this will put the image we just rendered into the visible window.
	// we want to wait on the _renderSemaphore for that,
	// as it's necessary that drawing commands have finished before the image is displayed to the user
	vk::PresentInfoKHR presentInfo = { };
	presentInfo.sType = vk::StructureType::ePresentInfoKHR;
	presentInfo.pNext = nullptr;

	presentInfo.pSwapchains = &swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;
	result = vkQueuePresentKHR((VkQueue) graphicsQueue,
			(VkPresentInfoKHR*) &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		resizing = true;
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		THROW_VK_EX(result);
	}

	if(result == VK_SUBOPTIMAL_KHR)
		resizing = true;
}

GraphicsManager_API_data::~GraphicsManager_API_data() {
	device.waitIdle();

	cleanupSwapchain();

	g_mgr->destroyEvent.invoke();

	device.destroy(def_renderPass);

	device.destroy(presentSemaphore);
	device.destroy(renderSemaphore);
	device.destroy(renderFence);

	device.destroy(commandPool);

	vkDestroyDevice(device, nullptr);
	device = nullptr;

	vkDestroySurfaceKHR(instance, surface, nullptr);
	surface = nullptr;

	vkb::destroy_debug_utils_messenger(instance, debug_messenger);
	debug_messenger = nullptr;

	instance.destroy();
	instance = nullptr;
}

NSP_GL_END
