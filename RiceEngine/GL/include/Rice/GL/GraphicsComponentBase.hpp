/*
 * GraphicsComponentBase.hpp
 *
 *  Created on: Apr 8, 2022
 *      Author: filebin
 */

#include "stdafx.hpp"

NSP_GL_BEGIN

class GraphicsComponentBase;

NSP_GL_END

#pragma once
#include "GraphicsManager.hpp"

NSP_GL_BEGIN

class GraphicsComponentBase : public ICleanable {
protected:
	Util::EventRegistration deleteReg;
	ptr<GraphicsManager> graphics_mgr;

	GraphicsManager_API_data& get_api_data();
public:
	GraphicsComponentBase(ptr<GraphicsManager> g_mgr, bool subscribe = true);
};

NSP_GL_END
