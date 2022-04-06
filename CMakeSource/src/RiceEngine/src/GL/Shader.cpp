#include "pch.h"

#include <Rice/GL/Shader.hpp>

NSP_GL_BEGIN

Shader::Shader(pGraphicsManager g_mgr) : GraphicsComponentBase(g_mgr) {}
Shader::~Shader() { cleanup(); }
void Shader::cleanup() {}

void Shader::loadShader(String path, Type type) {
	data_t buffer = Util::readFile(path);

	//buffer.insert(buffer.end(), {0,0,0,0});

	//create a new shader module, using the buffer we loaded
	vk::ShaderModuleCreateInfo createInfo = {};
	createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
	createInfo.pNext = nullptr;

	//codeSize has to be in bytes, so multiply the ints in the buffer by size of int to know the real size of the buffer
	createInfo.codeSize = buffer.size();
	createInfo.pCode = (uint*)buffer.data();

	//check that the creation goes well.
	vk::ShaderModule shaderModule;
	auto res = vkCreateShaderModule(getDevice(), (VkShaderModuleCreateInfo*)&createInfo, nullptr, (VkShaderModule*)&shaderModule);
	THROW_VK_EX_IF_BAD(res);

	switch(type) {
	case Type::Vertex:
		vertexShader = shaderModule;
		break;
	case Type::Fragment:
		fragmentShader = shaderModule;
		break;
	}
}

void Shader::buildPipeline(Vector2i size) {
	using help = VulkanHelper;

	//build the pipeline layout that controls the inputs/outputs of the shader
	//we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
	vk::PipelineLayoutCreateInfo pipeline_layout_info = help::pipeline_layout_create_info();
	auto res = vkCreatePipelineLayout(getDevice(), (VkPipelineLayoutCreateInfo*)&pipeline_layout_info, nullptr, (VkPipelineLayout*)&layout);

	//build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
	help::PipelineBuilder pipelineBuilder;

	pipelineBuilder.vk_shaderStages.push_back(
			help::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits::eVertex, vertexShader));

	pipelineBuilder.vk_shaderStages.push_back(
			help::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits::eFragment, fragmentShader));

	//vertex input controls how to read vertices from vertex buffers. We aren't using it yet
	pipelineBuilder.vk_vertexInputInfo = help::vertex_input_state_create_info();

	//input assembly is the configuration for drawing triangle lists, strips, or individual points.
	//we are just going to draw triangle list
	pipelineBuilder.vk_inputAssembly = help::input_assembly_create_info();

	//build viewport and scissor from the swapchain extents
	pipelineBuilder.vk_viewport.x = 0.0f;
	pipelineBuilder.vk_viewport.y = 0.0f;
	pipelineBuilder.vk_viewport.width = (float)size.x;
	pipelineBuilder.vk_viewport.height = (float)size.y;
	pipelineBuilder.vk_viewport.minDepth = 0.0f;
	pipelineBuilder.vk_viewport.maxDepth = 1.0f;

	pipelineBuilder.vk_scissor.offset = vk::Offset2D(0, 0);
	pipelineBuilder.vk_scissor.extent = vk::Extent2D(size.x, size.y);

	//configure the rasterizer to draw filled triangles
	pipelineBuilder.vk_rasterizer = help::rasterization_state_create_info();

	//we don't use multisampling, so just run the default one
	pipelineBuilder.vk_multisampling = help::multisampling_state_create_info();

	//a single blend attachment with no blending and writing to RGBA
	pipelineBuilder.vk_colorBlendAttachment = help::color_blend_attachment_state();

	//use the triangle layout we created
	pipelineBuilder.vk_pipelineLayout = layout;

	//finally build the pipeline
	pipeline = pipelineBuilder.build_pipeline(getDevice(), getDefRenderPass());
}

void Shader::setActive() {
	vkCmdBindPipeline(getMainCommandBuffer(), (VkPipelineBindPoint)vk::PipelineBindPoint::eGraphics, (VkPipeline)pipeline);
}
NSP_GL_END
