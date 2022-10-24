#include "../stdafx.hpp"

typedef struct SDL_Window SDL_Window;
typedef union SDL_Event SDL_Event;
typedef struct SDL_WindowEvent SDL_WindowEvent;

NSP_ENGINE_BEGIN

using WindowHandle = ptr<SDL_Window>;

class Window;

NSP_ENGINE_END

#pragma once
#include "InputManager.hpp"

NSP_ENGINE_BEGIN

struct DescWindow {
    static const int windowPosUndefined;
    String caption;
    int width{640};
    int height{480};
    bool resizing{true};
    int posx{windowPosUndefined};
    int posy{windowPosUndefined};
};

class Window : public enable_ptr<Window>, public ICleanable {
  public:
    typedef Util::Event<ptr<Window>> ResizeEvent;

  private:
    Window();

  public:
    static ptr<Window> create(DescWindow desc);
    ~Window() { cleanup(); }

    void join();
    void close() { cleanup(); }
    void cleanup() override;
    ptr<InputManager> getInputManager() { return inputmgr; }

    WindowHandle getHandle();
    int getLeft() const { return desc.posx; }
    int getTop() const { return desc.posy; }
    int getWidth() const {
        return getSize().x;
    }

    Vector2i getMousePos() const;

    int getHeight() const {
        return getSize().y;
    }

    Vector2i getSize() const;

    void setCursorShow(bool show);

    String getCaption() const { return desc.caption; }
    bool isExit() const { return is_exit; }
    bool isOpen() const { return !is_exit; }
    bool isActive() const { return is_active; }
    bool isMinimized() const;
    void updateWindowState();

    void grabMouse(bool grab);

  private:
    void update(std::stop_token stoken);
    void handleEvent(SDL_Event &e);
    void handleWindowEvent(SDL_WindowEvent &e);
    void setCursorPosition(Vector2i pos);

  public:
    ptr<ResizeEvent> resize_event = ResizeEvent::create();
  private:
    friend class InputManager;

    DescWindow desc;
    WindowHandle handle;
    ptr<InputManager> inputmgr;
    uptr<std::jthread> update_thread;
    std::shared_mutex update_mutex;
    bool is_exit = false;
    bool is_active = true;
    bool created = false;
};

NSP_ENGINE_END
