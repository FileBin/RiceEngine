﻿#include <Rice/Engine/Log.hpp>
#include <Rice/Engine/Window.hpp>

NSP_ENGINE_BEGIN

Window::Window()
    : inputmgr(nullptr), handle(nullptr), is_exit(false), is_active(true) {}

ptr<Window> Window::create(DescWindow desc) {
    ptr<Window> window = ptr<Window>(new Window());
    Log::debug("Creating window..");
    window->desc = desc;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Log::log(Log::Error, "SDL cannot be initialized: {}\n",
                 String(SDL_GetError()));
        THROW_EXCEPTION("SDL cannot be initialized!");
    }

    window->handle = WindowHandle{
        SDL_CreateWindow("VulkanTest", desc.posx, desc.posy, desc.width,
                         desc.height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN),
        SDL_DestroyWindow};

    window->created = true;

    window->inputmgr = InputManager::init(window->handle);

    SDL_SetWindowMinimumSize(window->handle.get(), 64, 64);

    if (!window->handle) {
        Log::log(Log::Error, "Could not create SDL window: {}\n",
                 String(SDL_GetError()));
        THROW_NULL_PTR_EXCEPTION(window->handle.get());
    }

    window->updateWindowState();

    Log::debug("Window successfully created!");
    return window;
}

bool Window::update() {
    SDL_Event event;
    SDL_PollEvent(&event);
    handleEvent(event);
    return !is_exit;
}

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
        inputmgr->eventKey((KeyCode)e.key.keysym.sym, true);
        break;
    case SDL_KEYUP:
        inputmgr->eventKey((KeyCode)e.key.keysym.sym, false);
        break;
    case SDL_MOUSEWHEEL:
        inputmgr->eventMouseWheel(e.wheel.y);
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

void Window::setInputMgr(ptr<InputManager> inputmgr) {
    this->inputmgr = inputmgr;
    updateWindowState();
}

bool Window::isResize() const {
    int w, h;
    SDL_GetWindowSize(handle.get(), &w, &h);
    return w != desc.width || h != desc.height;
}

bool Window::isMinimized() const {
    return SDL_GetWindowFlags(handle.get()) & SDL_WINDOW_MINIMIZED;
}

void Window::updateWindowState() {
    SDL_Rect rect;
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