#include "../stdafx.hpp"
NSP_GL_BEGIN
class GraphicsManager;
typedef RefPtr<GraphicsManager> pGraphicsManager;

struct GraphicsManager_API_data;
typedef RefPtr<GraphicsManager_API_data> pGraphicsManager_API_data;

class CommandBuffer;
typedef RefPtr<CommandBuffer> pCommandBuffer;

NSP_GL_END
#pragma once

#include "../Engine/Window.hpp"

NSP_GL_BEGIN
class_with_ref_ptr(GraphicsManager) better_implements(public ICleanable) {
public:
	typedef Event<> DestroyEvent;
	typedef Event<Vector2i> ResizeEvent;
private:
	friend class GraphicsComponentBase;
	friend struct GraphicsManager_API_data;
	bool is_initialized = false;

	EventRegistration resizeReg;

	pGraphicsManager_API_data api_data;

	pWindow window;

	bool isDrawing = false;
public:
    ResizeEvent resizePipelines, resizeRenderPasses, resizeCommandBuffers;
    DestroyEvent destroyEvent;

	GraphicsManager() = default;
	~GraphicsManager() override { cleanup(); }

	pWindow getWindow() { return window; }

	void init(pWindow window);

	void drawCmd(pCommandBuffer cmd);
	void cleanup() override;
};

NSP_GL_END
