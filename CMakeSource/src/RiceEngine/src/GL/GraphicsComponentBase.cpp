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

GraphicsManager_API_data& GraphicsComponentBase::get_api_data() { return *graphics_mgr->api_data; }

GraphicsComponentBase::GraphicsComponentBase(pGraphicsManager g_mgr) : graphics_mgr(g_mgr) {
	graphics_mgr->destroyEvent.subscribe(deleteReg, [this](){ cleanup(); }); // @suppress("Invalid arguments")
}

NSP_GL_END
