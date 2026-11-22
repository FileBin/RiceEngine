/*
 * api_Shader.hpp
 *
 *  Created on: Apr 27, 2022
 *      Author: root
 */

#pragma once

#include "Rice/GL/VertexLayout.hpp"
#include "VulkanHelper.hpp"
#include "api_GraphicsManager.hpp"
#include <Rice/Math/Vectors.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

struct Shader_API_Data {
    vk::ShaderModule vertexShader, fragmentShader, geometryShader, computeShader;
    vk::Pipeline pipeline;
    vk::PipelineLayout layout;
    vk::DescriptorSetLayout descriptorSetLayout;

    vec<vk::DescriptorSetLayoutBinding> bindings;

    void buildDescriptorSetLayout(GraphicsManager_API_data &api_data);
    void buildPipeline(GraphicsManager_API_data &api_data, Vector2i windowExcent,
                       VertexLayout &vertexLayout, uint vertexStride, bool blendEnable = false,
                       bool lineTopology = false, bool depthTest = true, bool wireframe = false,
                       bool invertBlend = false);
    void buildDepthOnlyPipeline(GraphicsManager_API_data &api_data, vk::RenderPass pass,
                                Vector2i extent, VertexLayout &vertexLayout, uint vertexStride);
    /* Compute pipeline: pipeline layout (the set layout only, no push constants,
     * no render pass, no vertex input, no viewport) + vkCreateComputePipelines. */
    void buildComputePipeline(GraphicsManager_API_data &api_data);
    void cleanupPipeline(GraphicsManager_API_data &api_data);
    void cleanup(GraphicsManager_API_data &api_data);
};

NSP_GL_END
