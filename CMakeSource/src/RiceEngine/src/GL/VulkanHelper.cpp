/*
 * VulkanHelper.cpp
 *
 *  Created on: 2 ���. 2022 �.
 *      Author: FileBinsLapTop
 */
#include "pch.h"
#include <Rice/GL/VulkanHelper.hpp>

NSP_GL_BEGIN

vk::CommandPoolCreateInfo VulkanHelper::command_pool_create_info(uint queueFamilyIndex, vk::CommandPoolCreateFlags flags) {
	vk::CommandPoolCreateInfo info = {};
	info.sType = vk::StructureType::eCommandPoolCreateInfo;
	info.pNext = nullptr;

	info.queueFamilyIndex = queueFamilyIndex;
	info.flags = flags;
	return info;
}

vk::CommandBufferAllocateInfo VulkanHelper::command_buffer_allocate_info(vk::CommandPool pool, uint count , vk::CommandBufferLevel level) {
	vk::CommandBufferAllocateInfo info = {};
	info.sType = vk::StructureType::eCommandBufferAllocateInfo;
	info.pNext = nullptr;

	info.commandPool = pool;
	info.commandBufferCount = count;
	info.level = level;
	return info;
}

vk::PipelineShaderStageCreateInfo VulkanHelper::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits stage, vk::ShaderModule module, const char* entry) {
	vk::PipelineShaderStageCreateInfo info{};
	info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
	info.pNext = nullptr;

	//shader stage
	info.stage = stage;
	//module containing the code for this shader stage
	info.module = module;
	//the entry point of the shader
	info.pName = entry;
	return info;
}

vk::PipelineVertexInputStateCreateInfo VulkanHelper::vertex_input_state_create_info(vec<vk::VertexInputBindingDescription>* bindings, vec<vk::VertexInputAttributeDescription>* attributes) {
	vk::PipelineVertexInputStateCreateInfo info = {};
	info.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
	info.pNext = nullptr;

	if(bindings && attributes) {
		info.vertexBindingDescriptionCount = bindings->size();
		info.pVertexBindingDescriptions = bindings->data();

		info.vertexAttributeDescriptionCount = attributes->size();
		info.pVertexAttributeDescriptions = attributes->data();
	} else {
		//no vertex bindings or attributes
		info.vertexBindingDescriptionCount = 0;
		info.pVertexBindingDescriptions = nullptr;
		info.vertexAttributeDescriptionCount = 0;
		info.pVertexAttributeDescriptions = nullptr;
	}
	return info;
}

vk::PipelineInputAssemblyStateCreateInfo VulkanHelper::input_assembly_create_info(vk::PrimitiveTopology topology) {
	vk::PipelineInputAssemblyStateCreateInfo info = {};
	info.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
	info.pNext = nullptr;

	info.topology = topology;
	info.primitiveRestartEnable = VK_FALSE;

	return info;
}

vk::PipelineRasterizationStateCreateInfo VulkanHelper::rasterization_state_create_info(vk::PolygonMode polygonMode, float line_width, vk::CullModeFlags cull_mode_flags, vk::FrontFace front_face) {
	vk::PipelineRasterizationStateCreateInfo info = {};
	info.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
	info.pNext = nullptr;

	info.depthClampEnable = VK_FALSE;
	//discards all primitives before the rasterization stage if enabled which we don't want
	info.rasterizerDiscardEnable = VK_FALSE;

	info.polygonMode = polygonMode;
	info.lineWidth = line_width;
	//no backface cull
	info.cullMode = cull_mode_flags;
	info.frontFace = front_face;
	//no depth bias
	info.depthBiasEnable = VK_FALSE;
	info.depthBiasConstantFactor = 0.0f;
	info.depthBiasClamp = 0.0f;
	info.depthBiasSlopeFactor = 0.0f;

	return info;
}

vk::PipelineMultisampleStateCreateInfo VulkanHelper::multisampling_state_create_info(vk::SampleCountFlagBits nSamples) {
	vk::PipelineMultisampleStateCreateInfo info = {};
	info.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
	info.pNext = nullptr;

	info.sampleShadingEnable = VK_FALSE;
	//multisampling defaulted to no multisampling (1 sample per pixel)
	info.rasterizationSamples = nSamples;
	info.minSampleShading = 1.0f;
	info.pSampleMask = nullptr;
	info.alphaToCoverageEnable = VK_FALSE;
	info.alphaToOneEnable = VK_FALSE;
	return info;
}

vk::PipelineColorBlendAttachmentState VulkanHelper::color_blend_attachment_state(vk::ColorComponentFlags colorMask, bool blendEnable) {
	vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = colorMask;
	colorBlendAttachment.blendEnable = TO_VK_BOOL(blendEnable);
	return colorBlendAttachment;
}

//TODO expand pipeline layout
vk::PipelineLayoutCreateInfo VulkanHelper::pipeline_layout_create_info() {
	vk::PipelineLayoutCreateInfo info {};
	info.sType = vk::StructureType::ePipelineLayoutCreateInfo;
	info.pNext = nullptr;

	//empty defaults
	info.flags = {};
	info.setLayoutCount = 0;
	info.pSetLayouts = nullptr;
	info.pushConstantRangeCount = 0;
	info.pPushConstantRanges = nullptr;
	return info;
}


vk::Pipeline VulkanHelper::PipelineBuilder::build_pipeline(vk::Device device, vk::RenderPass pass) {
	//make viewport state from our stored viewport and scissor.
	//at the moment we won't support multiple viewports or scissors
	vk::PipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
	viewportState.pNext = nullptr;

	viewportState.viewportCount = 1;
	viewportState.pViewports = &vk_viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &vk_scissor;

	//setup dummy color blending. We aren't using transparent objects yet
	//the blending is just "no blend", but we do write to the color attachment
	vk::PipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
	colorBlending.pNext = nullptr;

	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = vk::LogicOp::eCopy;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &vk_colorBlendAttachment;

	//build the actual pipeline
	//we now use all of the info structs we have been writing into into this one to create the pipeline
	vk::GraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
	pipelineInfo.pNext = nullptr;

	pipelineInfo.stageCount = vk_shaderStages.size();
	pipelineInfo.pStages = vk_shaderStages.data();
	pipelineInfo.pVertexInputState = &vk_vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &vk_inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &vk_rasterizer;
	pipelineInfo.pMultisampleState = &vk_multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = vk_pipelineLayout;
	pipelineInfo.renderPass = pass;
	pipelineInfo.subpass = 0;
	//pipelineInfo.basePipelineHandle = {};

	//it's easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
	vk::Pipeline newPipeline;
	auto res = vkCreateGraphicsPipelines((VkDevice)device, VK_NULL_HANDLE, 1, (VkGraphicsPipelineCreateInfo*)&pipelineInfo, nullptr, (VkPipeline*)&newPipeline);
	THROW_VK_EX_IF_BAD(res);
	return newPipeline;
}



NSP_GL_END
