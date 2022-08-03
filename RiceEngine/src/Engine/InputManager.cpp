#include "Rice/Engine/InputButtons.hpp"
#include "Rice/Engine/Log.hpp"
#include "Rice/Engine/Window.hpp"
#include "Rice/Math/Vectors/Vector2.hpp"
#include "Rice/Math/Vectors/Vector2i.hpp"
#include <Rice/Engine/InputManager.hpp>
#include <Rice/Math/Math.hpp>
#include <cstddef>
#include <mutex>
#include <shared_mutex>

NSP_ENGINE_BEGIN

ptr<InputManager> InputManager::create(ptr<Window> window) {
    auto inputmgr = ptr<InputManager>(new InputManager());
    inputmgr->window = window;
    return inputmgr;
}
InputManager::InputManager() {
    mouseRect = {};
    reset();
}

InputManager::~InputManager() {}

ptr<Window> InputManager::getWindow() {
    auto window_lock = window.lock();
    if (!window_lock)
        THROW_NULL_PTR_EXCEPTION(window_lock.get());
    return window_lock;
}

bool InputManager::getKey(KeyCode key) {
    return keyStates[static_cast<int>(key)];
}

Vector2 InputManager::getMousePos() { return mousePos; }

Vector2i InputManager::getMouseInfinitePos() {
    std::shared_lock lock(update_mutex);
    //Log::debug("Mouse infinite pos: (x: {} y: {})", mouseInfinitePos.x,
               //mouseInfinitePos.y);
    return mouseInfinitePos;
}

void InputManager::setMousePos(Vector2 pos) {
    getWindow()->setCursorPosition({(int)pos.x, (int)pos.y});
}

void InputManager::setMouseRect(Rect rect) {
    mouseRect.x = rect.x;
    mouseRect.y = rect.y;
    mouseRect.w = rect.w;
    mouseRect.h = rect.h;
}

Vector2 InputManager::getCenter() {
    Vector2 center;
    center.x = mouseRect.w;
    center.y = mouseRect.h;
    center *= .5;
    return center;
}

void InputManager::lockMouse() {
    if (getWindow()->isActive()) {
        if (!is_mouse_locked) {
            getWindow()->grabMouse(true);
            is_mouse_locked = true;
            //getWindow()->setCursorShow(false);
        }
    }
}

void InputManager::unlockMouse() {
    if (is_mouse_locked) {
        getWindow()->grabMouse(false);
        is_mouse_locked = false;
        //getWindow()->setCursorShow(true);
    }
}

void InputManager::update() {
    if (is_mouse_locked) {
        std::unique_lock lock(update_mutex);
        auto center = getCenter();
        mouseInfinitePos += mouseDelta;
        mouseDelta = {};
        if (getKey(KeyCode::Escape)) {
            unlockMouse();
        }
    }
}

void InputManager::eventCursor(Vector2 newpos, Vector2i delta) {
    mousePos = newpos;
    mouseDelta = delta;
}

void InputManager::eventKey(KeyCode key, bool rs_state) {
    keyStates[static_cast<int>(key)] = rs_state;
}

void InputManager::eventMouse(int mouse_key, bool rs_state) {
    mouseStates[mouse_key] = rs_state;
    switch (mouse_key) {
    case 1:
        eventKey(KeyCode::MouseLeft, rs_state);
        break;
    case 3:
        eventKey(KeyCode::MouseRight, rs_state);
        break;
    case 2:
        eventKey(KeyCode::MouseMiddle, rs_state);
        break;
    case 4:
        eventKey(KeyCode::XButton1, rs_state);
        break;
    case 5:
        eventKey(KeyCode::XButton2, rs_state);
        break;
    }
}

void InputManager::reset() {
    unlockMouse();
    keyStates.clear();
    mouseStates.clear();
}

// void InputManager::eventCursor(Vector2 newPos) { mousePos = newPos; }

void InputManager::eventMouseWheel(short value) {
    mouseScrollDelta = value - mouseWheel;
    mouseWheel = value;
}

NSP_ENGINE_END
