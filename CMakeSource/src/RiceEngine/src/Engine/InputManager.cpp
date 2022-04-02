#include "pch.h"
#include <Rice/Engine/InputManager.hpp>
#include <Rice/Math.hpp>

NSP_ENGINE_BEGIN

pInputManager InputManager::instance = nullptr;

pInputManager InputManager::init(WindowHandle _handle) {
	if (instance == nullptr) {
		instance = new InputManager();
		instance->handle = _handle;
		return instance;
	}
	THROW_EXCEPTION("Input is already initialized!");
}
InputManager::InputManager() {
	mouseRect = {};
	size_t n = (size_t) KeyCode::Max;
	keyStates = new bool[n];
	ZeroMemory(keyStates, n);

	mouseStates = new bool[10];
}

InputManager::~InputManager() {
	_DELETE_ARRAY(keyStates);
}

bool InputManager::getKey(KeyCode key) {
	if (instance->active)
		return instance->keyStates[(size_t) key];
	return false;
}

const Vector2 InputManager::getMousePos() {
	return instance->mousePos;
}

Vector2 InputManager::getMouseDelta() {
	if (!instance->is_mouse_locked)
		return Vector2::zero;
	return instance->mouseDelta;
}

void InputManager::setMousePos(Vector2 pos) {
	SDL_WarpMouseInWindow(instance->handle.get(), pos.x, pos.y);
	instance->mousePos = pos;
}

void InputManager::setMouseRect(SDL_Rect rect) {
	mouseRect.x = rect.x;
	mouseRect.y = rect.y;
	mouseRect.w = rect.w;
	mouseRect.h = rect.h;
}

void InputManager::lockMouse() {
	if (instance->active) {
		if (!instance->is_mouse_locked) {
			instance->is_mouse_locked = true;
			SDL_ShowCursor(0);
		}
	}
}

void InputManager::unlockMouse() {
	if (instance->active) {
		if (instance->is_mouse_locked) {
			instance->is_mouse_locked = false;
			SDL_ShowCursor(1);
		}
	}
}

void InputManager::setActive(bool active) {
	if (!active)
		unlockMouse();
	instance->active = active;
}

void InputManager::update() {
	if (active) {
		if (is_mouse_locked) {
			Vector2 center;
			center.x = mouseRect.w;
			center.y = mouseRect.h;
			center *= .5;
			setMousePos(center);
			if (getKey(KeyCode::Escape)) {
				unlockMouse();
			}
		}
	}

	mouseDelta = {0,0};
}

void InputManager::eventKey(KeyCode key, bool rs_state) {
	keyStates[static_cast<size_t>(key)] = rs_state;
}

void InputManager::eventCursor() {
	Vector2i pos;
	SDL_GetMouseState((int*)&pos.x, (int*)&pos.y);
	mousePos = (Vector2)pos;
}

void InputManager::eventMouseWheel(short value) {
	mouseScrollDelta = value - mouseWheel;
	mouseWheel = value;
}

NSP_ENGINE_END
