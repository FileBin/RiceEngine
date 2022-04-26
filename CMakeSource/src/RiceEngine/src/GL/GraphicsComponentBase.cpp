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

API_Data& GraphicsComponentBase::get_api_data() { return *graphics_mgr->api_data; }

NSP_GL_END
