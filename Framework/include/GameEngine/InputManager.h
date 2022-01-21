#pragma once
#include "stdafx.h"
#include "InputButtons.h"
#include "Vectors/Vector2.h"

namespace Game {
	class InputManager {
	public:
		static InputManager* PreInit();
		InputManager();
		~InputManager();

		static bool GetKey(KeyCode key);
		static const Vector2 GetMousePos();
		static Vector2 GetMouseDelta();
		static void SetMousePos(Vector2 pos);


		void SetRect(const RECT &winRect);
		void UpdateWindow(const UINT& msg, WPARAM wParam, LPARAM lParam);
		void Update();
		static void LockMouse();
		static void UnlockMouse();
		static void SetActive(bool active = true);
	private:
		bool active = true;
		bool lockMouse = false;
		bool* keyStates;
		bool* mouseStates;
		short mouseScrollDelta = 0, mouseWheel = 0;
		Vector2 mousePos = Vector2(), mouseDelta = Vector2();
		RECT windowRect;
		void eventKey(KeyCode key, bool rs_state);
		void eventCursor();
		void eventMouse(MouseKey key, bool rs_state);
		void eventMouseWheel(short value);
	};
}