#include "../stdafx.hpp"
#include "Rice/Math/Vectors/Vector2.hpp"
#include "Rice/Math/Vectors/Vector2i.hpp"
#include "Rice/defines.h"
#include "Rice/macros.h"
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
    Vector2 getMousePos();
    Vector2i getMouseInfinitePos();
    void setMousePos(Vector2 pos);

    void setMouseRect(Util::Rect rect);
    void update();
    void lockMouse();
    void unlockMouse();

  private:
    void reset();

    Vector2 getCenter();

    friend class Window;
    bool is_mouse_locked = false;
    map<int,bool> keyStates;
    map<int,bool> mouseStates;
    short mouseScrollDelta = 0, mouseWheel = 0;
    Vector2 mousePos = {};
    Vector2i mouseInfinitePos = {}, mouseDelta = {};
    wptr<Window> window;
    Util::Rect mouseRect;
    void eventKey(KeyCode key, bool rs_state);
    void eventCursor(Vector2 newPos, Vector2i delta);
    void eventMouse(int mouse_key, bool rs_state);
    void eventMouseWheel(short value);

    std::shared_mutex update_mutex;
};

NSP_ENGINE_END
