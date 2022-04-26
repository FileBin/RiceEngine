#include "pch.h"
#include <Rice/GL/GraphicsManager.hpp>

#include <Rice/Engine/Log.hpp>
#include <Rice/Math.hpp>

#include <Rice/GL/Shader.hpp>

#include "VulkanHelper.hpp"

#include "VulkanAPI_data.hpp"

NSP_GL_BEGIN

void GraphicsManager::init(pWindow _window) {
	Log::debug("GraphicsManager initializing...");
	window = _window;
	api_data = new API_Data();

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
	api_data->instance = vkb_inst.instance;

	//store the debug messenger
#ifdef _DEBUG
	api_data->debug_messenger = vkb_inst.debug_messenger;
#endif

	if(!SDL_Vulkan_CreateSurface(window->getHandle().get(), api_data->instance, (VkSurfaceKHR*)&api_data->surface)){
		THROW_EXCEPTION("Failed to create vulkan surface! (Maybe window is not created)");
	}

	if(!api_data->surface) THROW_NULL_PTR_EXCEPTION(nullptr);

	//use vkbootstrap to select a GPU.
	//We want a GPU that can write to the SDL surface and supports Vulkan 1.1
	vkb::PhysicalDeviceSelector selector {vkb_inst};

	auto dev_result = selector
			.set_minimum_version(1, 1)
			.set_surface(api_data->surface)
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
	api_data->device = vkbDevice.device;

	api_data->GPU = physicalDevice.physical_device;
	Log::log(Log::Info, "GPU INFO: {}", String(api_data->GPU.getProperties().deviceName.data()));

	// use vkbootstrap to get a Graphics queue
	api_data->graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	api_data->graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	init_swapchain();
	init_commands();
	init_def_renderpass();
	init_framebuffers();
	init_sync_structures();

	//register_events();

	is_initialized = true;

	Log::debug("Successfully initialized!");

}

void GraphicsManager::init_swapchain() {
	Log::debug("Swapchain building...");
	vkb::SwapchainBuilder swapchainBuilder {api_data->GPU, api_data->device, api_data->surface};

	if(use_hdr) {
		swapchainBuilder.set_desired_format(
				(VkSurfaceFormatKHR)vk::SurfaceFormatKHR(
						vk::Format::eR16G16B16A16Sfloat,
						vk::ColorSpaceKHR::eHdr10HlgEXT));
	} else {
		swapchainBuilder.use_default_format_selection();
	}

	uint w,h;
	w = window->getWidth();
	h = window->getHeight();

	if(w >= 64 && h >= 64) {
		for (size_t i = 0; i < api_data->swapchainImageViews.size(); i++) {
			api_data->device.destroy(api_data->swapchainImageViews[i]);
		}

		api_data->windowExcent.width = w;
		api_data->windowExcent.height = h;
		vkb::Swapchain vkbSwapchain = swapchainBuilder
		.set_desired_present_mode((VkPresentModeKHR)vk::PresentModeKHR::eFifo) //use v-sync present mode
		//.set_desired_extent(windowExcent.width, windowExcent.height)
		.set_old_swapchain((VkSwapchainKHR) api_data->swapchain)
		.build()
		.value();

		//store swapchain and its related images
		api_data->swapchain = vkbSwapchain.swapchain;
		api_data->swapchainImages = (vec<vk::Image>&)vkbSwapchain.get_images().value();
		api_data->swapchainImageViews = (vec<vk::ImageView>&)vkbSwapchain.get_image_views().value();

		api_data->swapchainImageFormat = (vk::Format&)vkbSwapchain.image_format;
	}
	Log::debug("Swapchain built!");
}

void GraphicsManager::init_commands() {
	//create a command pool for commands submitted to the graphics queue.
	//we also want the pool to allow for resetting of individual command buffers
	auto commandPoolInfo = VulkanHelper::command_pool_create_info(api_data->graphicsQueueFamily, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

	api_data->commandPool = api_data->device.createCommandPool(commandPoolInfo);

	//allocate the default command buffer that we will use for rendering
	auto cmdAllocInfo = VulkanHelper::command_buffer_allocate_info(api_data->commandPool);


	api_data->mainCommandBuffer = api_data->device.allocateCommandBuffers(cmdAllocInfo)[0];
	//res = (vk::Result)vkAllocateCommandBuffers(api_data->device, (VkCommandBufferAllocateInfo*)&cmdAllocInfo, (VkCommandBuffer*)&api_data->mainCommandBuffer);
	//THROW_api_data->EX_IF_BAD(res);
}

void GraphicsManager::init_def_renderpass(){
	vk::AttachmentDescription color_attachment;
	//the attachment will have the format needed by the swapchain
	color_attachment.format = api_data->swapchainImageFormat;
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

	auto res = api_data->device.createRenderPass(&render_pass_info, nullptr, &api_data->def_renderPass);
	THROW_VK_EX_IF_BAD(res);
}

void GraphicsManager::init_framebuffers() {
	//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	vk::FramebufferCreateInfo fb_info = {};

	fb_info.renderPass = api_data->def_renderPass;
	fb_info.attachmentCount = 1;
	fb_info.width = api_data->windowExcent.width;
	fb_info.height = api_data->windowExcent.height;
	fb_info.layers = 1;

	//grab how many images we have in the swapchain
	const uint32_t swapchain_imagecount = api_data->swapchainImages.size();
	api_data->framebuffers = vec<vk::Framebuffer>(swapchain_imagecount);

	//create framebuffers for each of the swapchain image views
	for (int i = 0; i < swapchain_imagecount; i++) {
		fb_info.pAttachments = &api_data->swapchainImageViews[i];
		api_data->framebuffers[i] = api_data->device.createFramebuffer(fb_info);
	}
}

void GraphicsManager::init_sync_structures() {
	//create synchronization structures
	vk::FenceCreateInfo fenceCreateInfo = {};

	//we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
	fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

	api_data->renderFence = api_data->device.createFence(fenceCreateInfo);

	//for the semaphores we don't need any flags
	vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.flags = vk::SemaphoreCreateFlags();

	api_data->presentSemaphore = api_data->device.createSemaphore(semaphoreCreateInfo);
	api_data->renderSemaphore = api_data->device.createSemaphore(semaphoreCreateInfo);
}

void GraphicsManager::recreateSwapChain() {
	if(window->isMinimized()) {
		Log::debug("Window is minimized!");
		return;
	}
	Log::debug("Swapchain recreating...");
	api_data->device.waitIdle();
	cleanupSwapChain(false);

	init_swapchain();
	init_def_renderpass();
	init_framebuffers();
	resizeEvent.invoke({ api_data->windowExcent.width, api_data->windowExcent.height });
}

void GraphicsManager::cleanupSwapChain(bool destroy) {
	for (size_t i = 0; i < api_data->framebuffers.size(); i++) {
		api_data->device.destroy(api_data->framebuffers[i]);
	}

	api_data->device.destroy(api_data->def_renderPass);

	if(destroy) {
		for (size_t i = 0; i < api_data->swapchainImages.size(); i++) {
			api_data->device.destroy(api_data->swapchainImageViews[i]);
		}

		api_data->swapchainImageViews.clear();

		api_data->device.destroy(api_data->swapchain);
	}
}

bool GraphicsManager::beginDraw() {
	if(isDrawing) return false;
	isDrawing = true;
	//wait until the GPU has finished rendering the last frame. Timeout of 0.1 second (fast)
	auto res = api_data->device.waitForFences({ api_data->renderFence }, true, 100000000);
	if(res == vk::Result::eTimeout && !window->isMinimized()) {
		//wait until the GPU has finished rendering the last frame. Timeout of 1 second (slow)
		res = api_data->device.waitForFences({ api_data->renderFence }, true, 1000000000);
	}
	THROW_VK_EX_IF_BAD(res);

	api_data->device.resetFences({ api_data->renderFence });

	api_data->mainCommandBuffer.reset();

	//request image from the swapchain, 1 second timeout
	res = api_data->device.acquireNextImageKHR(api_data->swapchain, 1000000000, api_data->presentSemaphore, nullptr, &swapchainImageIndex); // @suppress("Ambiguous problem")
	if(res == vk::Result::eErrorOutOfDateKHR) {
		recreateSwapChain();
		isDrawing = false;
		return false;
	}
	THROW_VK_EX_IF_BAD(res);


	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
	vk::CommandBufferBeginInfo cmdBeginInfo = {};
	cmdBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
	cmdBeginInfo.pNext = nullptr;

	cmdBeginInfo.pInheritanceInfo = nullptr;
	cmdBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	api_data->mainCommandBuffer.begin(cmdBeginInfo);

	//make a clear-color from frame number. This will flash with a 120*pi frame period.
	vk::ClearValue clearValue;
	Vector3f flash =
			{
					abs(sin(frameNumber / 51.f)), // @suppress("Invalid arguments")
					abs(sin(frameNumber / 43.f)), // @suppress("Invalid arguments")
					abs(sin(frameNumber / 13.f)), // @suppress("Invalid arguments")
			};
	clearValue.color.float32[0] = flash.x;
	clearValue.color.float32[1] = flash.y;
	clearValue.color.float32[2] = flash.z;
	clearValue.color.float32[3] = 1.f;

	//start the main renderpass.
	//We will use the clear color from above, and the framebuffer of the index the swapchain gave us
	vk::RenderPassBeginInfo rpInfo = {};

	//windowExcent.width = window->getWidth();
	//windowExcent.height = window->getHeight();

	rpInfo.renderPass = api_data->def_renderPass;
	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	rpInfo.renderArea.extent = api_data->windowExcent;
	rpInfo.framebuffer = api_data->framebuffers[swapchainImageIndex];

	//connect clear values
	rpInfo.clearValueCount = 1;
	rpInfo.pClearValues = &clearValue;

	api_data->mainCommandBuffer.beginRenderPass(&rpInfo, vk::SubpassContents::eInline);

	return true;
}

void GraphicsManager::draw(uint count) {
	api_data->mainCommandBuffer.draw(count, 1, 0, 0);
}

void GraphicsManager::endDraw() {
	if(!isDrawing) return;

	//finalize the render pass
	api_data->mainCommandBuffer.endRenderPass();
	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	api_data->mainCommandBuffer.end();

	isDrawing = false;

	//prepare the submission to the queue.
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished

	vk::SubmitInfo submit = {};
	submit.sType = vk::StructureType::eSubmitInfo;
	submit.pNext = nullptr;

	vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &api_data->presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &api_data->renderSemaphore;

	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &api_data->mainCommandBuffer;

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	auto res = vkQueueSubmit((VkQueue)api_data->graphicsQueue, 1, (VkSubmitInfo*)&submit, (VkFence)api_data->renderFence);

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

	presentInfo.pSwapchains = &api_data->swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &api_data->renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	res = vkQueuePresentKHR((VkQueue)api_data->graphicsQueue, (VkPresentInfoKHR*)&presentInfo);

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
	if(is_initialized) {
		api_data->device.waitIdle();
		Log::debug("Graphics manager cleanup...");

		cleanupSwapChain();

		destroyEvent.invoke();

		api_data->device.destroy(api_data->presentSemaphore);
		api_data->device.destroy(api_data->renderSemaphore);
		api_data->device.destroy(api_data->renderFence);

		api_data->device.destroy(api_data->commandPool);

		vkDestroyDevice(api_data->device, nullptr);
		api_data->device = nullptr;

		vkDestroySurfaceKHR(api_data->instance, api_data->surface, nullptr);
		api_data->surface = nullptr;

		vkb::destroy_debug_utils_messenger(api_data->instance, api_data->debug_messenger);
		api_data->debug_messenger = nullptr;

		api_data->instance.destroy();
		api_data->instance = nullptr;

		api_data.release();
	}
	is_initialized = false;
}

NSP_GL_END
