#pragma once

#include "InputButtons.h"

namespace GameFramework {
	class MouseEvent {
		MouseEvent(int nx, int ny) : x(nx), y(ny) {}

		// координаты мыши
		int x;
		int y;
	};

	// событие нажати€ кнопки мыши
	class MouseEventClick : public MouseEvent {
		MouseEventClick(MouseKey b, int nx, int ny) : MouseEvent(nx, ny), btn(b) {}
		const MouseKey btn;	//  лавиша
	};

	// событие прокрутки мыши
	class MouseEventWheel : public MouseEvent {
		MouseEventWheel(int nwheel, int nx, int ny) : MouseEvent(nx, ny), wheel(nwheel) {}

		int wheel;
	};

	// событие клавиши
	class KeyEvent {
		KeyEvent(wchar_t c, KeyCode kc) : wc(c), code(kc) {}

		const wchar_t wc;
		const KeyCode code;
	};

}
