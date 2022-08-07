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

  public:
    static ptr<GraphicsManager> create();

    ~GraphicsManager() override;

    ptr<Window> getWindow() { return window; }

    void init(ptr<Window> window);
    void update();

    void sync();

    void executeCmd(ptr<CommandBuffer> cmd);
    void executeCmds(vec<ptr<CommandBuffer>> cmds);
    void cleanup() override;

  public:
    ptr<ResizeEvent> resizePipelines = ResizeEvent::create(),
                     resizeRenderPasses = ResizeEvent::create(),
                     resizeCommandBuffers = ResizeEvent::create(),
                     resizeGraphicsComponents = ResizeEvent::create();
    ptr<DestroyEvent> destroyCommandBuffers = DestroyEvent::create();
    ptr<DestroyEvent> destroyEvent = DestroyEvent::create();

  private:
    Util::EventRegistration resizeReg;

    uptr<GraphicsManager_API_data> api_data;

    ptr<Window> window;

    bool is_drawing = false;
    bool is_initialized = false;
};

NSP_GL_END
