#include "pch.h"
#include <Rice/GL/GraphicsManager.hpp>

#include <Rice/Engine/Log.hpp>
#include <Rice/GL/VulkanHelper.hpp>
#include <Rice/Math.hpp>

#include <Rice/GL/Shader.hpp>

NSP_GL_BEGIN

void GraphicsManager::init(pWindow _window) {
	Log::debug("GraphicsManager initializing...");
	window = _window;

	vkb::InstanceBuilder builder;

	//make the Vulkan instance
	auto inst_ret = builder.set_app_name("Riced Field")
#ifdef _DEBUG
	.request_validation_layers(true)
#endif
	.require_api_version(1, 1, 0)
#ifdef _DEBUG
	.use_default_debug_messenger()
#endif
	.build();

	if(!inst_ret.has_value())
		THROW_VK_EX(inst_ret.vk_result());

	vkb::Instance vkb_inst = inst_ret.value();

	//store the instance
	vk_instance = vkb_inst.instance;

	//store the debug messenger
#ifdef _DEBUG
	vk_debug_messenger = vkb_inst.debug_messenger;
#endif

	if(!SDL_Vulkan_CreateSurface(window->getHandle().get(), vk_instance, (VkSurfaceKHR*)&vk_surface)){
		THROW_EXCEPTION("Failed to create vulkan surface! (Maybe window is not created)");
	}

	if(!vk_surface) THROW_NULL_PTR_EXCEPTION(nullptr);

	//use vkbootstrap to select a GPU.
	//We want a GPU that can write to the SDL surface and supports Vulkan 1.1
	vkb::PhysicalDeviceSelector selector {vkb_inst};

	auto dev_result = selector
			.set_minimum_version(1, 1)
			.set_surface(vk_surface)
			.prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
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
	vk_device = vkbDevice.device;

	vk_GPU = physicalDevice.physical_device;
	Log::log(Log::Info, "GPU INFO: {}", String(vk_GPU.getProperties().deviceName.data()));

	// use vkbootstrap to get a Graphics queue
	vk_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	vk_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	init_swapchain();
	init_commands();
	init_def_renderpass();
	init_framebuffers();
	init_sync_structures();

	register_events();

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
	.set_desired_present_mode((VkPresentModeKHR)vk::PresentModeKHR::eFifo) //use v-sync present mode
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
	THROW_VK_EX_IF_BAD(res);

	//allocate the default command buffer that we will use for rendering
	auto cmdAllocInfo = VulkanHelper::command_buffer_allocate_info(vk_commandPool);

	res = (vk::Result)vkAllocateCommandBuffers(vk_device, (VkCommandBufferAllocateInfo*)&cmdAllocInfo, (VkCommandBuffer*)&vk_mainCommandBuffer);
	THROW_VK_EX_IF_BAD(res);
}

void GraphicsManager::init_def_renderpass(){
	vk::AttachmentDescription color_attachment;
	//the attachment will have the format needed by the swapchain
	color_attachment.format = vk_swapchainImageFormat;
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

	VkAttachmentReference color_attachment_ref = {};
	//attachment number will index into the pAttachments array in the parent renderpass itself
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = (VkImageLayout)vk::ImageLayout::eColorAttachmentOptimal;

	//we are going to create 1 subpass, which is the minimum you can do
	vk::SubpassDescription subpass = {};
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = (vk::AttachmentReference*)&color_attachment_ref;

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	//connect the color attachment to the info
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = (VkAttachmentDescription*)&color_attachment;
	//connect the subpass to the info
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = (VkSubpassDescription*)&subpass;

	auto res = vkCreateRenderPass(vk_device, &render_pass_info, nullptr, (VkRenderPass*)&vk_def_renderPass);
	THROW_VK_EX_IF_BAD(res);
}

void GraphicsManager::init_framebuffers() {
	//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	vk::FramebufferCreateInfo fb_info = {};
	fb_info.sType = vk::StructureType::eFramebufferCreateInfo;
	fb_info.pNext = nullptr;

	fb_info.renderPass = vk_def_renderPass;
	fb_info.attachmentCount = 1;
	fb_info.width = window->getWidth();
	fb_info.height = window->getHeight();
	fb_info.layers = 1;

	//grab how many images we have in the swapchain
	const uint32_t swapchain_imagecount = vk_swapchainImages.size();
	vk_framebuffers = vec<vk::Framebuffer>(swapchain_imagecount);

	//create framebuffers for each of the swapchain image views
	for (int i = 0; i < swapchain_imagecount; i++) {
		fb_info.pAttachments = &vk_swapchainImageViews[i];
		auto res = vkCreateFramebuffer((VkDevice)vk_device, (VkFramebufferCreateInfo*)&fb_info,
				nullptr, (VkFramebuffer*)&vk_framebuffers[i]);
		THROW_VK_EX_IF_BAD(res);
	}
}

void GraphicsManager::init_sync_structures() {
	//create synchronization structures
	vk::FenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
	fenceCreateInfo.pNext = nullptr;

	//we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
	fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

	auto res = vkCreateFence((VkDevice)vk_device, (VkFenceCreateInfo*)&fenceCreateInfo, nullptr, (VkFence*)&vk_renderFence);
	THROW_VK_EX_IF_BAD(res);

	//for the semaphores we don't need any flags
	vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = vk::SemaphoreCreateFlags();

	res = vkCreateSemaphore(vk_device,
			(VkSemaphoreCreateInfo*)&semaphoreCreateInfo, nullptr,
			(VkSemaphore*)&vk_presentSemaphore);
	THROW_VK_EX_IF_BAD(res);

	res = vkCreateSemaphore(vk_device,
			(VkSemaphoreCreateInfo*)&semaphoreCreateInfo, nullptr,
			(VkSemaphore*)&vk_renderSemaphore);
	THROW_VK_EX_IF_BAD(res);
}

void GraphicsManager::register_events() {
	on_resize_uuid = window->resize_event.subscribe([this](pWindow sender){ onResize(sender); }); // @suppress("Invalid arguments")
}

void GraphicsManager::onResize(pWindow win) {
	recreateSwapChain();
}

void GraphicsManager::recreateSwapChain() {
	auto res = vkDeviceWaitIdle(vk_device);
	THROW_VK_EX_IF_BAD(res);
	cleanupSwapChain();

	init_swapchain();
	init_def_renderpass();
	init_framebuffers();
}

void GraphicsManager::cleanupSwapChain() {
	for (size_t i = 0; i < vk_framebuffers.size(); i++) {
		vkDestroyFramebuffer(vk_device, vk_framebuffers[i], nullptr);
	}

	resizeEvent.invoke(window);

	vkDestroyRenderPass(vk_device, vk_def_renderPass, nullptr);

	for (size_t i = 0; i < vk_swapchainImages.size(); i++) {
		vkDestroyImageView(vk_device, vk_swapchainImageViews[i], nullptr);
		//vkDestroyImage(vk_device, vk_swapchainImages[i], nullptr);
	}

	vk_swapchainImageViews.clear();
	//vk_swapchainImages.clear();

	vkDestroySwapchainKHR(vk_device, vk_swapchain, nullptr);
}

bool GraphicsManager::beginDraw() {
	if(window->isResize())
		return false;
	if(isDrawing) return false;
	isDrawing = true;
	//wait until the GPU has finished rendering the last frame. Timeout of 1 second
	int res = vkWaitForFences(vk_device, 1, (VkFence*)&vk_renderFence, true, 1000000000);
	THROW_VK_EX_IF_BAD(res);
	//if(res != 0) return false;

	res = vkResetFences(vk_device, 1, (VkFence*)&vk_renderFence);
	THROW_VK_EX_IF_BAD(res);
	//if(res != 0) return false;


	//request image from the swapchain, one second timeout
	res = vkAcquireNextImageKHR(vk_device, vk_swapchain, 1000000000, vk_presentSemaphore, nullptr, &swapchainImageIndex);
	THROW_VK_EX_IF_BAD(res);
	//if(res != 0) return false;

	res = vkResetCommandBuffer(vk_mainCommandBuffer, 0);
	THROW_VK_EX_IF_BAD(res);
	//if(res != 0) return false;


	//naming it cmd for shorter writing
	vk::CommandBuffer cmd = vk_mainCommandBuffer;

	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
	vk::CommandBufferBeginInfo cmdBeginInfo = {};
	cmdBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
	cmdBeginInfo.pNext = nullptr;

	cmdBeginInfo.pInheritanceInfo = nullptr;
	cmdBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	res = vkBeginCommandBuffer(cmd, (VkCommandBufferBeginInfo*)&cmdBeginInfo);
	THROW_VK_EX_IF_BAD(res);
	//if(res != 0) return false;

	//make a clear-color from frame number. This will flash with a 120*pi frame period.
	VkClearValue clearValue;
	Vector3f flash =
			{
					abs(sin(frameNumber / 101.f)), // @suppress("Invalid arguments")
					abs(sin(frameNumber / 79.f)), // @suppress("Invalid arguments")
					abs(sin(frameNumber / 93.f)), // @suppress("Invalid arguments")
			};
	clearValue.color = { { flash.x, flash.y, flash.z, 1.0f } };

	//start the main renderpass.
	//We will use the clear color from above, and the framebuffer of the index the swapchain gave us
	VkRenderPassBeginInfo rpInfo = {};
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpInfo.pNext = nullptr;

	VkExtent2D windowExcent;

	windowExcent.height = window->getHeight();
	windowExcent.width = window->getWidth();

	rpInfo.renderPass = vk_def_renderPass;
	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	rpInfo.renderArea.extent = windowExcent;
	rpInfo.framebuffer = vk_framebuffers[swapchainImageIndex];

	//connect clear values
	rpInfo.clearValueCount = 1;
	rpInfo.pClearValues = &clearValue;

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	//if(res == 0)
		return true;
	//return false;
}

void GraphicsManager::draw(uint count) {
	vkCmdDraw(vk_mainCommandBuffer, count, 1, 0, 0);
}

void GraphicsManager::endDraw() {
	if(!isDrawing) return;
	isDrawing = false;
	vk::CommandBuffer cmd = vk_mainCommandBuffer;
	//finalize the render pass
	vkCmdEndRenderPass(cmd);
	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	auto res = vkEndCommandBuffer(cmd);
	THROW_VK_EX_IF_BAD(res);


	//prepare the submission to the queue.
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished

	vk::SubmitInfo submit = {};
	submit.sType = vk::StructureType::eSubmitInfo;
	submit.pNext = nullptr;

	vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &vk_presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &vk_renderSemaphore;

	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	res = vkQueueSubmit((VkQueue)vk_graphicsQueue, 1, (VkSubmitInfo*)&submit, (VkFence)vk_renderFence);

	if (res == VK_ERROR_OUT_OF_DATE_KHR) {
	    recreateSwapChain();
	    return;
	} else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
	    THROW_VK_EX(res);
	}
	// this will put the image we just rendered into the visible window.
	// we want to wait on the _renderSemaphore for that,
	// as it's necessary that drawing commands have finished before the image is displayed to the user
	vk::PresentInfoKHR presentInfo = {};
	presentInfo.sType = vk::StructureType::ePresentInfoKHR;
	presentInfo.pNext = nullptr;

	presentInfo.pSwapchains = &vk_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &vk_renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	res = vkQueuePresentKHR((VkQueue)vk_graphicsQueue, (VkPresentInfoKHR*)&presentInfo);

	if (res == VK_ERROR_OUT_OF_DATE_KHR) {
	    recreateSwapChain();
	    return;
	} else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
	    THROW_VK_EX(res);
	}

	//increase the number of frames drawn
	frameNumber++;

}


//FINALIZER
void GraphicsManager::cleanup() {
	window->resize_event.unsubscribe(on_resize_uuid);
	if(is_initialized) {
		Log::debug("Graphics manager cleanup...");

		vkDestroyCommandPool(vk_device, vk_commandPool, nullptr);

		cleanupSwapChain();

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
