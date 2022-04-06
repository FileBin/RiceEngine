/*
 * GraphicsComponentBase.cpp
 *
 *  Created on: Apr 10, 2022
 *      Author: filebin
 */

#include "pch.h"
#include <Rice/GL/GraphicsComponentBase.hpp>

#include <Rice/GL/GraphicsManager.hpp>

NSP_GL_BEGIN

vk::Device& GraphicsComponentBase::getDevice() { return graphics_mgr->vk_device; }
vk::RenderPass& GraphicsComponentBase::getDefRenderPass() { return graphics_mgr->vk_def_renderPass; }
vk::CommandBuffer& GraphicsComponentBase::getMainCommandBuffer() { return graphics_mgr->vk_mainCommandBuffer; }

NSP_GL_END
