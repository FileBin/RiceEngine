#include <GameEngine/InputManager.h>
#include <GameEngine/macros.h>

namespace GameEngine {

	InputManager* instance = nullptr;

	InputManager* InputManager::Init() {
		if (instance == nullptr) {
			instance = new InputManager();
			return instance;
		}
		throw std::exception("Input is already initialized!");
	}
	InputManager::InputManager() {
		windowRect = RECT();
		keyStates = new bool[(size_t)KeyCode::Max];
		mouseStates = new bool[10];
	}

	InputManager::~InputManager() {
		_DELETE_ARRAY(keyStates);
	}

	inline bool InputManager::GetKey(KeyCode key) {
		return instance->keyStates[(size_t)key];
	}

	const Vector2 InputManager::GetMousePos() {
		return instance->mousePos;
	}

	void InputManager::SetRect(const RECT &winRect) {
		windowRect.left = winRect.left;
		windowRect.right = winRect.right;
		windowRect.top = winRect.top;
		windowRect.bottom = winRect.bottom;
	}

	void InputManager::UpdateWindow(const UINT& msg, WPARAM wParam, LPARAM lParam) {
		mouseScrollDelta = 0;
		mouseDelta = Vector2::zero();
		KeyCode KeyIndex;

		eventCursor();// событие движения мыши
		switch (msg) {
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
			break;
		case WM_LBUTTONUP:
			eventMouse(MouseKey::Left, false);
			break;
		case WM_RBUTTONDOWN:
			eventMouse(MouseKey::Right, true);
			break;
		case WM_RBUTTONUP:
			eventMouse(MouseKey::Right, false);
			break;
		case WM_MBUTTONDOWN:
			eventMouse(MouseKey::Middle, true);
			break;
		case WM_MBUTTONUP:
			eventMouse(MouseKey::Middle, false);
			break;
		case WM_MOUSEWHEEL:
			eventMouseWheel((short)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
			break;
		}
	}

	void InputManager::eventKey(KeyCode key, bool state) {
		keyStates[static_cast<size_t>(key)] = state;
	}

	void InputManager::eventCursor() {
		POINT Position;
		GetCursorPos(&Position);	// получаем текущую позицию курсора

		Position.x -= windowRect.left;
		Position.y -= windowRect.top;

		if (mousePos.x == Position.x && mousePos.y == Position.y)
			return;
		mouseDelta.x = Position.x;
		mouseDelta.y = Position.y;

		mouseDelta = mouseDelta - mousePos;

		mousePos.x = Position.x;
		mousePos.y = Position.y;
	}

	void InputManager::eventMouse(MouseKey key, bool state) {
		mouseStates[static_cast<size_t>(key)] = state;
	}

	void InputManager::eventMouseWheel(short value) {
		mouseScrollDelta = value - mouseWheel;
		mouseWheel = value;
	}
}