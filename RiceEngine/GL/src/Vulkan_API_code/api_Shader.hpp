/*
 * api_Shader.hpp
 *
 *  Created on: Apr 27, 2022
 *      Author: root
 */

#pragma once

#include "Rice/GL/VertexLayout.hpp"
#include "Rice/Math/Vectors/Vector2i.hpp"
#include "VulkanHelper.hpp"
#include "api_GraphicsManager.hpp"
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

struct Shader_API_Data {
	vk::ShaderModule vertexShader, fragmentShader, geometryShader;
	vk::Pipeline pipeline;
	vk::PipelineLayout layout;
	vk::DescriptorSetLayout descriptorSetLayout;

	vec<vk::DescriptorSetLayoutBinding> bindings;

    void buildDescriptorSetLayout(GraphicsManager_API_data &api_data);
    void buildPipeline(GraphicsManager_API_data& api_data, Vector2i windowExcent, VertexLayout& vertexLayout, uint vertexStride);
    void cleanupPipeline(GraphicsManager_API_data &api_data);
    void cleanup(GraphicsManager_API_data &api_data);
};

NSP_GL_END
