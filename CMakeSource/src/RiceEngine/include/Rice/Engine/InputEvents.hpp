#pragma once

#include "InputButtons.h"

namespace Game {
	class MouseEvent {
		MouseEvent(int nx, int ny) : x(nx), y(ny) {}

		// ���������� ����
		int x;
		int y;
	};

	// ������� ������� ������ ����
	class MouseEventClick : public MouseEvent {
		MouseEventClick(MouseKey b, int nx, int ny) : MouseEvent(nx, ny), btn(b) {}
		const MouseKey btn;	// �������
	};

	// ������� ��������� ����
	class MouseEventWheel : public MouseEvent {
		MouseEventWheel(int nwheel, int nx, int ny) : MouseEvent(nx, ny), wheel(nwheel) {}

		int wheel;
	};

	// ������� �������
	class KeyEvent {
		KeyEvent(wchar_t c, KeyCode kc) : wc(c), code(kc) {}

		const wchar_t wc;
		const KeyCode code;
	};

}
