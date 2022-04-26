#include "../stdafx.hpp"
NSP_GL_BEGIN
class GraphicsManager;
typedef SmartPtr<GraphicsManager> pGraphicsManager;
NSP_GL_END
#pragma once

#include "../Engine/Window.hpp"

#include "API_Data.hpp"

NSP_GL_BEGIN

class GraphicsManager : public ICleanable {
public:
	typedef Event<> DestroyEvent;
	typedef Event<Vector2i> ResizeEvent;
private:
	friend class GraphicsComponentBase;
	bool is_initialized = false;

	pAPI_data api_data;

	uint frameNumber = 0;
	uint swapchainImageIndex = 0;

	pWindow window;

	bool isDrawing = false;

    bool use_hdr =
#ifdef USE_HDR
    		true;
#else
    		false;
#endif
public:
    ResizeEvent resizeEvent;
    DestroyEvent destroyEvent;

	GraphicsManager() = default;
	~GraphicsManager() override { cleanup(); }

	pWindow getWindow() { return window; }

	void init(pWindow window);

	bool beginDraw();
	void draw(uint count);
	void endDraw();
	void cleanup() override;

private:
	void init_swapchain();
	void init_commands();
	void init_def_renderpass();
	void init_framebuffers();
	void init_sync_structures();

	void cleanupSwapChain(bool destroy = true);
	void recreateSwapChain();
};

NSP_GL_END
