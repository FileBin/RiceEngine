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

	GraphicsManager_API_data& get_api_data();
public:
	GraphicsComponentBase(pGraphicsManager g_mgr);
};

NSP_GL_END
