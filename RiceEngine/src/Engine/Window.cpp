#include "Rice/Engine/InputManager.hpp"
#include "Rice/Math/Vectors/Vector2i.hpp"
#include "SDL2/SDL_hints.h"
#include "SDL2/SDL_mouse.h"
#include "SDL2/SDL_video.h"
#include <Rice/Engine/Log.hpp>
#include <Rice/Engine/Window.hpp>
#include <chrono>

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_vulkan.h>

#include <stop_token>
#include <thread>

NSP_ENGINE_BEGIN

const int DescWindow::windowPosUndefined = SDL_WINDOWPOS_UNDEFINED;

bool Window::isMinimized() const {
    return SDL_GetWindowFlags(handle.get()) & SDL_WINDOW_MINIMIZED;
}

Vector2i Window::getMousePos() const {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return Vector2i(x, y);
}

Vector2i Window::getSize() const {
    Vector2i size;
    SDL_GetWindowSize(handle.get(), (int *)&size.x, (int *)&size.y);
    return size;
}

WindowHandle Window::getHandle() { return handle; }

void Window::setCursorShow(bool show) { SDL_ShowCursor(show ? 1 : 0); }

void Window::setCursorPosition(Vector2i pos) {
    SDL_WarpMouseInWindow(handle.get(), pos.x, pos.y);
}

void Window::grabMouse(bool grab) {
    auto grabbed = grab ? SDL_TRUE : SDL_FALSE;
    // use grab warping because grab by raw mouse data works strange
    SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1");
    SDL_SetRelativeMouseMode(grabbed);
}

Window::Window() : inputmgr(nullptr), handle(nullptr), is_exit(false), is_active(true) {}

ptr<Window> Window::create(DescWindow desc) {
    ptr<Window> window = ptr<Window>(new Window());
    Log::debug("Creating window..");
    window->desc = desc;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Log::log(Log::Error, "SDL cannot be initialized: {}\n", String(SDL_GetError()));
        THROW_EXCEPTION("SDL cannot be initialized!");
    }

    window->handle = WindowHandle{
        SDL_CreateWindow(desc.caption.toUTF8String().c_str(), desc.posx, desc.posy, desc.width,
                         desc.height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN),
        SDL_DestroyWindow};

    window->created = true;

    window->inputmgr = InputManager::create(window);

    // set minimum window size to avoid problems with Vulkan
    SDL_SetWindowMinimumSize(window->handle.get(), 64, 64);

    if (!window->handle) {
        Log::log(Log::Error, "Could not create SDL window: {}\n", String(SDL_GetError()));
        THROW_NULL_PTR_EXCEPTION(window->handle.get());
    }

    window->updateWindowState();

    // start thread for window update
    window->update_thread.reset(
        new std::jthread([window](std::stop_token stoken) { window->update(stoken); }));

    Log::debug("Window successfully created!");
    return window;
}

void Window::update(std::stop_token stoken) {
    while (true) {
        using namespace std::chrono;
        using namespace std::chrono_literals;
        auto tickStart = steady_clock::now();
        auto tickLength = 5ms; // 200tps
        SDL_Event event;
        SDL_PollEvent(&event);
        handleEvent(event);

        inputmgr->update();

        if (is_exit || stoken.stop_requested()) {
            is_exit = true;
            return;
        }

        std::this_thread::sleep_until(tickStart + tickLength);
    }
}

void Window::join() { update_thread->join(); }

void Window::cleanup() {
    if (created) {
        is_exit = true;
        is_active = false;
        handle = nullptr;
        Log::debug("Window closed!");
    }
    created = false;
}

void Window::handleEvent(SDL_Event &e) {
    updateWindowState();
    switch (e.type) {
    case SDL_WINDOWEVENT:
        handleWindowEvent(e.window);
        break;
    case SDL_KEYDOWN:
        inputmgr->eventKey((KeyCode)e.key.keysym.scancode, true);
        break;
    case SDL_KEYUP:
        inputmgr->eventKey((KeyCode)e.key.keysym.scancode, false);
        break;
    case SDL_MOUSEWHEEL:
        inputmgr->eventMouseWheel(e.wheel.y);
        break;
    case SDL_MOUSEMOTION:
        inputmgr->eventCursor({(double)e.motion.x, (double)e.motion.y},
                              {e.motion.xrel, e.motion.yrel});
        break;
    case SDL_MOUSEBUTTONDOWN:
        inputmgr->eventMouse(e.button.button, true);
        break;
    case SDL_MOUSEBUTTONUP:
        inputmgr->eventMouse(e.button.button, false);
        break;
    }
}

void Window::handleWindowEvent(SDL_WindowEvent &e) {
    switch (e.event) {
    case SDL_WINDOWEVENT_FOCUS_GAINED:
    case SDL_WINDOWEVENT_SHOWN:
        is_active = true;
        break;
    case SDL_WINDOWEVENT_HIDDEN:
    case SDL_WINDOWEVENT_FOCUS_LOST:
        is_active = false;
        inputmgr->reset();
        break;
    case SDL_WINDOWEVENT_CLOSE:
        is_exit = true;
        break;
    case SDL_WINDOWEVENT_RESIZED:
        resize_event->invoke(shared_from_this());
        updateWindowState();
        break;
    case SDL_WINDOWEVENT_MOVED:
        break;
    }
}

void Window::updateWindowState() {
    Util::Rect rect;
    SDL_GetWindowPosition(handle.get(), &rect.x, &rect.y);
    SDL_GetWindowSize(handle.get(), &rect.w, &rect.h);
    desc.posx = rect.x;
    desc.posy = rect.y;
    desc.width = rect.w;
    desc.height = rect.h;
    if (inputmgr) {
        inputmgr->setMouseRect(rect);
    }
}

NSP_ENGINE_END
