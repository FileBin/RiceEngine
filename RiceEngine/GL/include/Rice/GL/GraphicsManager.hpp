#include "Rice/defines.h"
#include "stdafx.hpp"
NSP_GL_BEGIN
class GraphicsManager;

struct GraphicsManager_API_data;

class CommandBuffer;

NSP_GL_END
#pragma once

#include "Rice/Engine/Window.hpp"

NSP_GL_BEGIN
class GraphicsManager : public enable_ptr<GraphicsManager>, public ICleanable {
  public:
    typedef Util::Event<> DestroyEvent;
    typedef Util::Event<Vector2i> ResizeEvent;

  private:
    GraphicsManager();
    friend class GraphicsComponentBase;
    friend struct GraphicsManager_API_data;
    bool is_initialized = false;

    Util::EventRegistration resizeReg;

    uptr<GraphicsManager_API_data> api_data;

    ptr<Window> window;

    bool isDrawing = false;

  public:
    ptr<ResizeEvent> resizePipelines = ResizeEvent::create(),
                     resizeRenderPasses = ResizeEvent::create(),
                     resizeCommandBuffers = ResizeEvent::create();
    ptr<DestroyEvent> destroyEvent = DestroyEvent::create();

    static ptr<GraphicsManager> create();

    ~GraphicsManager() override;

    ptr<Window> getWindow() { return window; }

    void init(ptr<Window> window);

    void executeCmd(ptr<CommandBuffer> cmd);
    void executeCmds(vec<ptr<CommandBuffer>> cmds);
    void cleanup() override;
};

NSP_GL_END
