#pragma once
#include "stdafx.h"
#include "InputButtons.h"
#include "Math.h"
namespace GameEngine {
	class InputManager {
	public:
		static InputManager* Init();
		InputManager();
		~InputManager();

		static bool GetKey(KeyCode key);
		static const Vector2 GetMousePos();

		void SetRect(const RECT &winRect);
		void UpdateWindow(const UINT& msg, WPARAM wParam, LPARAM lParam);
	private:
		bool* keyStates;
		bool* mouseStates;
		short mouseScrollDelta = 0, mouseWheel = 0;
		Vector2 mousePos = Vector2(), mouseDelta = Vector2();
		RECT windowRect;
		void eventKey(KeyCode key, bool state);
		void eventCursor();
		void eventMouse(MouseKey key, bool state);
		void eventMouseWheel(short value);
	};
}