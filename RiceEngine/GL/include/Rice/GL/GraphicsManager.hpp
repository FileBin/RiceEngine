#include "Rice/Util/Event.hpp"
#include "stdafx.hpp"
#include <functional>
#include <thread>
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

    /* Pixel size of the swapchain images: the size a CommandBuffer::captureColor()
     * destination buffer has to hold (width * height * 4 bytes). */
    Vector2i getFrameSize();

    /* Submits `cmd` (and presents). Returns false when no frame was submitted
     * because the swapchain was out of date - a test that has to know whether a
     * frame really happened cannot tell otherwise. */
    bool executeCmd(ptr<CommandBuffer> cmd);
    bool executeCmds(vec<ptr<CommandBuffer>> cmds);
    void cleanup() override;

  public:
    ptr<ResizeEvent> resizePipelines = ResizeEvent::create(),
                     resizeRenderPasses = ResizeEvent::create(),
                     resizeCommandBuffers = ResizeEvent::create(),
                     resizeSubCommandBuffers = ResizeEvent::create(),
                     resizeGraphicsComponents = ResizeEvent::create();
    ptr<DestroyEvent> destroyCommandBuffers = DestroyEvent::create();
    ptr<DestroyEvent> destroyEvent = DestroyEvent::create();
    ptr<DestroyEvent> destroyOneTimeBuffers = DestroyEvent::create();

  private:
    Util::EventRegistration resizeReg;

    uptr<GraphicsManager_API_data> api_data;

    ptr<Window> window;

    void startUpdateThread();
    static void updateThreadFunc(ptr<GraphicsManager> self, std::stop_token token);

    bool is_drawing = false;
    bool is_initialized = false;
};

NSP_GL_END
