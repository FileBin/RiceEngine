/*
 * GraphicsComponentBase.hpp
 *
 *  Created on: Apr 8, 2022
 *      Author: filebin
 */

#include "../stdafx.hpp"

NSP_GL_BEGIN

class GraphicsComponentBase;
typedef SmartPtr<GraphicsComponentBase> pGraphicsComponentBase;

NSP_GL_END

#pragma once
#include "GraphicsManager.hpp"

NSP_GL_BEGIN

class GraphicsComponentBase : public ICleanable {
protected:
	pGraphicsManager graphics_mgr;

	vk::Device& getDevice();
	vk::RenderPass& getDefRenderPass();
	vk::CommandBuffer& getMainCommandBuffer();
public:
	GraphicsComponentBase(pGraphicsManager g_mgr) :
		graphics_mgr(g_mgr)
	{}
};
NSP_GL_END
