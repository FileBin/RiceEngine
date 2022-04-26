#include "pch.h"
#include <Rice/GL/GraphicsManager.hpp>

#include <Rice/Engine/Log.hpp>
#include <Rice/Math.hpp>

#include <Rice/GL/Shader.hpp>

#include "VulkanHelper.hpp"

#include "VulkanAPI_data_impl.hpp"

NSP_GL_BEGIN

void GraphicsManager::init(pWindow _window) {
	Log::debug("GraphicsManager initializing...");
	window = _window;
	api_data = new API_Data(this);

	//register_events();

	is_initialized = true;

	Log::debug("Successfully initialized!");

}

bool GraphicsManager::beginDraw() {
	if(isDrawing) return false;
	isDrawing = true;

	if(!api_data->beginDraw()){
		isDrawing = false;
		return false;
	}

	return true;
}

void GraphicsManager::draw(uint count) {
	api_data->draw(count);
}

void GraphicsManager::endDraw() {
	if(!isDrawing) return;

	isDrawing = false;

	api_data->endDraw();

	//increase the number of frames drawn
	frameNumber++;

}


//FINALIZER
void GraphicsManager::cleanup() {
	if(is_initialized) {
		Log::debug("Graphics manager cleanup...");
		api_data.release();
	}
	is_initialized = false;
}

NSP_GL_END
