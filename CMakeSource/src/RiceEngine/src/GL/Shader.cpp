
#include "pch.h"
#include <Rice/GL/Shader.hpp>
#include <Rice/GL/GraphicsComponentBase.hpp>

#include "Vulkan_API_code/api_GraphicsManager.hpp"
#include "Vulkan_API_code/api_Shader.hpp"

NSP_GL_BEGIN

Shader::Shader(pGraphicsManager g_mgr) : GraphicsComponentBase(g_mgr) {
	api_data = new Shader_API_Data();
	on_resize_uuid = graphics_mgr->resizePipelines.subscribe([this](Vector2i win){ onResize(win); }); // @suppress("Invalid arguments")
}
Shader::~Shader() { cleanup(); }

void Shader::cleanup() {
	if(init) {
		cleanupShaders();
		cleanupPipeline();

		graphics_mgr->resizePipelines.unsubscribe(on_resize_uuid);
		api_data.release();
	}
	init = false;
}

void Shader::cleanupShaders() {
	if(api_data->vertexShader)
		get_api_data().device.destroy(api_data->vertexShader);
	if(api_data->fragmentShader)
		get_api_data().device.destroy(api_data->fragmentShader);
	if(api_data->geometryShader)
		get_api_data().device.destroy(api_data->geometryShader);
}

void Shader::cleanupPipeline() {
	if(api_data->pipeline)
		get_api_data().device.destroy(api_data->pipeline);
    if(api_data->layout)
    	get_api_data().device.destroy(api_data->layout);
}

void Shader::loadShader(String path, Type type) {
	data_t buffer = Util::readFile(path);

	//create a new shader module, using the buffer we loaded
	vk::ShaderModuleCreateInfo createInfo = {};
	createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
	createInfo.pNext = nullptr;

	//codeSize has to be in bytes, so multiply the ints in the buffer by size of int to know the real size of the buffer
	createInfo.codeSize = buffer.size();
	createInfo.pCode = (uint*)buffer.data();

	//check that the creation goes well.
	vk::ShaderModule shaderModule = get_api_data().device.createShaderModule(createInfo);

	switch(type) {
	case Type::Vertex:
		api_data->vertexShader = shaderModule;
		break;
	case Type::Fragment:
		api_data->fragmentShader = shaderModule;
		break;
	case Type::Geometry:
		api_data->geometryShader = shaderModule;
	}
}

void Shader::buildPipeline(Vector2i extent) {
	using help = VulkanHelper;

	//build the pipeline layout that controls the inputs/outputs of the shader
	//we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
	vk::PipelineLayoutCreateInfo pipeline_layout_info = help::pipeline_layout_create_info();
	api_data->layout = get_api_data().device.createPipelineLayout(pipeline_layout_info);

	//build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
	help::PipelineBuilder pipelineBuilder;

	if(api_data->vertexShader)
		pipelineBuilder.vk_shaderStages.push_back(
				help::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits::eVertex, api_data->vertexShader));

	if(api_data->fragmentShader)
		pipelineBuilder.vk_shaderStages.push_back(
				help::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits::eFragment, api_data->fragmentShader));

	if(api_data->geometryShader)
		pipelineBuilder.vk_shaderStages.push_back(
				help::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits::eGeometry, api_data->geometryShader));

	//vertex input controls how to read vertices from vertex buffers. We aren't using it yet
	pipelineBuilder.vk_vertexInputInfo = help::vertex_input_state_create_info();

	//input assembly is the configuration for drawing triangle lists, strips, or individual points.
	//we are just going to draw triangle list
	pipelineBuilder.vk_inputAssembly = help::input_assembly_create_info();

	//build viewport and scissor from the swapchain extents
	pipelineBuilder.vk_viewport.x = 0.0f;
	pipelineBuilder.vk_viewport.y = 0.0f;
	pipelineBuilder.vk_viewport.width = (float)extent.x;
	pipelineBuilder.vk_viewport.height = (float)extent.y;
	pipelineBuilder.vk_viewport.minDepth = 0.0f;
	pipelineBuilder.vk_viewport.maxDepth = 1.0f;

	pipelineBuilder.vk_scissor.offset = vk::Offset2D(0, 0);
	pipelineBuilder.vk_scissor.extent = vk::Extent2D(extent.x,extent.y);

	//configure the rasterizer to draw filled triangles
	pipelineBuilder.vk_rasterizer = help::rasterization_state_create_info();

	//we don't use multisampling, so just run the default one
	pipelineBuilder.vk_multisampling = help::multisampling_state_create_info();

	//a single blend attachment with no blending and writing to RGBA
	pipelineBuilder.vk_colorBlendAttachment = help::color_blend_attachment_state();

	//use the triangle layout we created
	pipelineBuilder.vk_pipelineLayout = api_data->layout;

	//finally build the pipeline
	api_data->pipeline = pipelineBuilder.build_pipeline(get_api_data().device, get_api_data().def_renderPass);
	init = true;
}

void Shader::onResize(Vector2i extent) {
	cleanupPipeline();
	buildPipeline(extent);
}

NSP_GL_END
