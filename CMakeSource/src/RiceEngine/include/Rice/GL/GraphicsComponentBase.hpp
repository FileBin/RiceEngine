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
#include "API_Data.hpp"

NSP_GL_BEGIN

class GraphicsComponentBase : public ICleanable {
protected:
	pGraphicsManager graphics_mgr;

	GraphicsManager::DestroyEvent::UUID destroy_uuid;

	API_Data& get_api_data();
public:
	GraphicsComponentBase(pGraphicsManager g_mgr) :
		graphics_mgr(g_mgr)
	{
		destroy_uuid = graphics_mgr->destroyEvent.subscribe([this](){ cleanup(); }); // @suppress("Invalid arguments")
	}

	virtual ~GraphicsComponentBase() {
		graphics_mgr->destroyEvent.unsubscribe(destroy_uuid);
	}
};
NSP_GL_END
