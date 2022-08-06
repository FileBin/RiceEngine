/*
 * api_CommandBuffer.hpp
 *
 *  Created on: 27 ���. 2022 �.
 *      Author: FileBinsLapTop
 */

#pragma once

#include "VulkanHelper.hpp"
#include "api_GraphicsManager.hpp"

#include <Rice/GL/CommandBuffer.hpp>
#include <vulkan/vulkan_handles.hpp>

NSP_GL_BEGIN

struct DescriptorSetCreator;

struct CommandBuffer_API_data {
    vec<vk::CommandBuffer> cmd;
    vk::DescriptorSet descriptorSet;
    vk::DescriptorPool descriptorPool;
    bool begin_pass;

    uint bufCount();

    void build(GraphicsManager_API_data &api_data);

    void bindDescriptosSets(GraphicsManager_API_data &api_data,
                            DescriptorSetCreator &creator, uint i);
    void createDescriptorSet(GraphicsManager_API_data &api_data,
                             DescriptorSetCreator &creator);

    void begin(GraphicsManager_API_data &api_data, vk::Extent2D window, uint i);
    void end(uint i);
    void reset(uint i);

    void doCommand(ptr<CommandBuffer::Command> command, uint i,
                   GraphicsManager_API_data &api_data,
                   DescriptorSetCreator &creator);

    void cleanup(GraphicsManager_API_data &api_data);
};

NSP_GL_END
