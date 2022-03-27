﻿#include "pch.h"
#include <GameEngine/InputManager.h>
#include <GameEngine/macros.h>
#include <GameEngine/Util/exceptions/exception.h>

namespace Game {

	InputManager* instance = nullptr;

	InputManager* InputManager::PreInit() {
		if (instance == nullptr) {
			instance = new InputManager();
			return instance;
		}
		THROW_EXCEPTION("Input is already initialized!");
	}
	InputManager::InputManager() {
		windowRect = RECT();
		size_t n = (size_t)KeyCode::Max;
		keyStates = new bool[n];
		ZeroMemory(keyStates, n);

		mouseStates = new bool[10];
	}

	InputManager::~InputManager() {
		_DELETE_ARRAY(keyStates);
	}

	bool InputManager::GetKey(KeyCode key) {
		if (instance->active)
			return instance->keyStates[(size_t)key];
		return false;
	}

	const Vector2 InputManager::GetMousePos() {
		return instance->mousePos;
	}

	Vector2 InputManager::GetMouseDelta() {
		if (!instance->lockMouse) return Vector2::zero;
		return instance->mouseDelta;
	}

	void InputManager::SetMousePos(Vector2 pos) {
		SetCursorPos(pos.x + instance->windowRect.left, pos.y + instance->windowRect.top);
		instance->mousePos = pos;
	}

	void InputManager::SetWindowRect(const RECT& winRect) {
		windowRect.left = winRect.left;
		windowRect.right = winRect.right;
		windowRect.top = winRect.top;
		windowRect.bottom = winRect.bottom;
	}

	void InputManager::SetClientRect(const RECT& rect) {
		clientRect.left = rect.left;
		clientRect.right = rect.right;
		clientRect.top = rect.top;
		clientRect.bottom = rect.bottom;
	}

	void InputManager::UpdateWindow(const UINT& msg, WPARAM wParam, LPARAM lParam) {
		mouseScrollDelta = 0;
		KeyCode KeyIndex;
		eventCursor();
		switch (msg) {
		case WM_MOUSEMOVE:
			break;
		case WM_KEYDOWN:
			KeyIndex = static_cast<KeyCode>(wParam);
			eventKey(KeyIndex, true);
			break;
		case WM_KEYUP:
			KeyIndex = static_cast<KeyCode>(wParam);
			eventKey(KeyIndex, false);
			break;
		case WM_LBUTTONDOWN:
 			eventMouse(MouseKey::Left, true);
			eventKey(KeyCode::MouseLeft, true);
			break;
		case WM_LBUTTONUP:
			eventMouse(MouseKey::Left, false);
			eventKey(KeyCode::MouseLeft, false);
			break;
		case WM_RBUTTONDOWN:
			eventMouse(MouseKey::Right, true);
			eventKey(KeyCode::MouseRight, true);
			break;
		case WM_RBUTTONUP:
			eventMouse(MouseKey::Right, false);
			eventKey(KeyCode::MouseRight, false);
			break;
		case WM_MBUTTONDOWN:
			eventMouse(MouseKey::Middle, true);
			eventKey(KeyCode::MouseMiddle, true);
			break;
		case WM_MBUTTONUP:
			eventMouse(MouseKey::Middle, false);
			eventKey(KeyCode::MouseMiddle, false);
			break;
		case WM_MOUSEWHEEL:
			eventMouseWheel((short)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
			break;
		case WM_INPUT:
		{
			mouseDelta = Vector2::zero;
			UINT size = 0;
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
			LPBYTE buf = new BYTE[size];
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buf, &size, sizeof(RAWINPUTHEADER));
			PRAWINPUT raw = (RAWINPUT*)buf;
			mouseDelta = { (double)raw->data.mouse.lLastX, (double)raw->data.mouse.lLastY };
			mouseDelta = mouseDelta;
			//Log::Debug(L"({}, {})", mouseDelta.x, mouseDelta.y);
			delete[] buf;
			buf = 0;
		}
		break;
		}
	}

	void InputManager::LockMouse() {
		if (instance->active) {
			if (!instance->lockMouse) {
				instance->lockMouse = true;
				ShowCursor(false);
			}
		}
	}

	void InputManager::UnlockMouse() {
		if (instance->active) {
			if (instance->lockMouse) {
				instance->lockMouse = false;
				ShowCursor(true);
			}
		}
	}

	void InputManager::SetActive(bool active) {
		if (!active) UnlockMouse();
		instance->active = active;
	}

	void InputManager::Update(){
		if (active) {
			if (lockMouse) {
				Vector2 center;
				center.x = windowRect.right - windowRect.left;
				center.y = windowRect.bottom - windowRect.top;
				center *= .5;
				SetMousePos(center);
				if (GetKey(KeyCode::Escape)) {
					UnlockMouse();
				}
			}
		}

		mouseDelta = { 0,0 };
	}


	void InputManager::eventKey(KeyCode key, bool rs_state) {
		keyStates[static_cast<size_t>(key)] = rs_state;
	}

	void InputManager::eventCursor() {
		POINT Position{0};
		GetCursorPos(&Position);

		ScreenToClient(hwnd, &Position);

		mousePos.x = Position.x;
		mousePos.y = Position.y;
	}

	void InputManager::eventMouse(MouseKey key, bool rs_state) {
		mouseStates[static_cast<size_t>(key)] = rs_state;
	}

	void InputManager::eventMouseWheel(short value) {
		mouseScrollDelta = value - mouseWheel;
		mouseWheel = value;
	}
}