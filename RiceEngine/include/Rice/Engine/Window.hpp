#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

using WindowHandle = ptr<SDL_Window>;

class Window;

NSP_ENGINE_END

#pragma once
#include "InputManager.hpp"

NSP_ENGINE_BEGIN

struct DescWindow {
    String caption{L"Window"};
    int width{640};
    int height{480};
    bool resizing{true};
    int posx{SDL_WINDOWPOS_UNDEFINED};
    int posy{SDL_WINDOWPOS_UNDEFINED};
};

class Window : public ICleanable {
  public:
    typedef Event<Window> ResizeEvent;

    Window();
    ~Window() { cleanup(); }

    Window(const Window &other) = default;

    bool create(DescWindow desc);
    bool update();
    void close() { cleanup(); }
    void cleanup() override;
    void setInputMgr(ptr<InputManager> inputmgr);

    WindowHandle getHandle() { return handle; }
    int getLeft() const { return desc.posx; }
    int getTop() const { return desc.posy; }
    int getWidth() const {
        int x, y;
        SDL_GetWindowSize(handle.cget(), &x, &y);
        return x;
    }
    int getHeight() const {
        int x, y;
        SDL_GetWindowSize(handle.cget(), &x, &y);
        return y;
    }

    Vector2i getSize() const {
        Vector2i size;
        SDL_GetWindowSize(handle.cget(), (int *)&size.x, (int *)&size.y);
        return size;
    }

    String getCaption() const { return desc.caption; }
    bool isExit() const { return is_exit; }
    bool isActive() const { return is_active; }
    bool isResize() const;
    bool isMinimized() const;
    void updateWindowState();

  private:
    void handleEvent(SDL_Event & e);
    void handleWindowEvent(SDL_WindowEvent & e);

  public:
    ResizeEvent resize_event;

  private:
    friend class Core;

    DescWindow desc;
    WindowHandle handle;
    bool is_exit;
    bool is_active;
    bool created = false;
    pInputManager inputmgr;
};

NSP_ENGINE_END
