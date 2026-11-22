#include "../stdafx.hpp"

#include "Rice/Util/macros.h"
#include <Rice/Math/Vectors.hpp>
#include <shared_mutex>

NSP_ENGINE_BEGIN

class InputManager;

NSP_ENGINE_END

#pragma once
#include "InputButtons.hpp"
#include "Window.hpp"

NSP_ENGINE_BEGIN

class InputManager {
  public:
    static ptr<InputManager> create(ptr<Window> window);
    InputManager();
    ~InputManager();

    ptr<Window> getWindow();

    bool getKey(KeyCode key);

    // Scroll wheel delta accumulated since the last call, consumed on read: the
    // wheel arrives as many small events per frame and SDL reports an accumulator,
    // not a delta, so both have to be folded into one number per frame.
    int consumeWheelDelta();
    Vector2 getMousePos();
    Vector2i getMouseInfinitePos();
    void setMousePos(Vector2 pos);

    void setMouseRect(Util::Rect rect);
    void update();
    void lockMouse();
    void unlockMouse();
    bool isMouseLocked() const { return is_mouse_locked; }

  private:
    void reset();

    Vector2 getCenter();

    friend class Window;
    bool is_mouse_locked = false;
    map<int, bool> keyStates;
    map<int, bool> mouseStates;
    short mouseScrollDelta = 0, mouseWheel = 0;
    int wheelAccum = 0;
    Vector2 mousePos = {};
    Vector2i mouseInfinitePos = {}, mouseDelta = {};
    // Every motion event of the current frame; `mouseDelta` used to be overwritten
    // per event, so all but the last event of a frame were dropped.
    Vector2i pendingDelta = {};
    wptr<Window> window;
    Util::Rect mouseRect;
    void eventKey(KeyCode key, bool rs_state);
    void eventCursor(Vector2 newPos, Vector2i delta);
    void eventMouse(int mouse_key, bool rs_state);
    void eventMouseWheel(short value);

    std::shared_mutex update_mutex;
};

NSP_ENGINE_END
