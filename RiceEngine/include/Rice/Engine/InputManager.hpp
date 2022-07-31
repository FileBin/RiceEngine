#include "../stdafx.hpp"
#include "Rice/Math/Vectors/Vector2.hpp"
#include "Rice/macros.h"

NSP_ENGINE_BEGIN

class InputManager;

NSP_ENGINE_END

#pragma once
#include "InputButtons.hpp"
#include "Window.hpp"

NSP_ENGINE_BEGIN

class InputManager {
public:
	static ptr<InputManager> init(WindowHandle handle);
	InputManager();
	~InputManager();

	static bool getKey(KeyCode key);
	static const Vector2 getMousePos();
	static Vector2 getMouseDelta();
	static void setMousePos(Vector2 pos);

	void setMouseRect(SDL_Rect rect);
	void update();
	static void lockMouse();
	static void unlockMouse();
	static void setActive(bool active = true);
private:
	static ptr<InputManager> instance;

	friend class Window;
	bool active = true;
	bool is_mouse_locked = false;
	bool* keyStates;
	bool* mouseStates;
	short mouseScrollDelta = 0, mouseWheel = 0;
	Vector2 mousePos = {}, mouseDelta = {};
	WindowHandle handle;
	SDL_Rect mouseRect;
	void eventKey(KeyCode key, bool rs_state);
	void eventCursor();
	void eventMouse(KeyCode key, bool rs_state);
	void eventMouseWheel(short value);
};

NSP_ENGINE_END
