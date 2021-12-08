#pragma once

namespace GameEngine {
	enum class MouseKey {
		Left = 0,
		Middle,
		Right,
	};
	enum class KeyCode {
		MouseLeft = 0x01, // Left mouse button
		MouseRight = 0x02, // Right mouse button
		Cancel = 0x03, // Control-break processing
		MouseMiddle = 0x04, // Middle mouse button (three-button mouse)
		XButton1 = 0x05, // X1 mouse button
		XButton2 = 0x06, // X2 mouse button
		Backspace = 0x08, // BACKSPACE key
		Tab = 0x09, // TAB key
		Clear = 0x0C, // CLEAR key
		Return = 0x0D, // ENTER key
		Shift = 0x10, // SHIFT key
		Control = 0x11, // CTRL key
		Alt = 0x12, // ALT key
		Pause = 0x13, // PAUSE key
		Capital = 0x14, // CAPS LOCK key
		Kana = 0x15, // IME Kana mode
		Hanguel = 0x15, // IME Hanguel mode
		Hangul = 0x15, // IME Hangul mode
		Junja = 0x17, // IME Junja mode
		Final = 0x18, // IME final mode
		Hanja = 0x19, // IME Hanja mode
		Kanji = 0x19, // IME Kanji mode

		Escape = 0x1B,
		Space = 0x20,
		PageUp = 0x21,
		PageDown = 0x22,
		End = 0x23,
		Home = 0x24,
		Left = 0x25, // left arrow key
		Up = 0x26, // up arrow key
		Right = 0x27, // right arrow key
		Down = 0x28, // down arrow key
		Select = 0x29,
		Exe = 0x2b, // execute key
		Snapshot = 0x2C,
		Insert = 0x2D,
		Delete = 0x2E,
		Help = 0x2f,

		Button0 = 0x30,
		Button1 = 0x31,
		Button2 = 0x32,
		Button3 = 0x33,
		Button4 = 0x34,
		Button5 = 0x35,
		Button6 = 0x36,
		Button7 = 0x37,
		Button8 = 0x38,
		Button9 = 0x39,

		a = 0x41,
		B = 0x42,
		C = 0x43,
		D = 0x44,
		E = 0x45,
		F = 0x46,
		G = 0x47,
		H = 0x48,
		I = 0x49,
		J = 0x4a,
		K = 0x4b,
		L = 0x4c,
		M = 0x4d,
		N = 0x4e,
		O = 0x4f,
		P = 0x50,
		Q = 0x51,
		R = 0x52,
		S = 0x53,
		T = 0x54,
		U = 0x55,
		V = 0x56,
		W = 0x57,
		X = 0x58,
		Y = 0x59,
		Z = 0x5a,

		Winleft = 0x5B,
		Winright = 0x5C,
		Apps = 0x5d,

		Numpad0 = 0x60,
		Numpad1 = 0x61,
		Numpad2 = 0x62,
		Numpad3 = 0x63,
		Numpad4 = 0x64,
		Numpad5 = 0x65,
		Numpad6 = 0x66,
		Numpad7 = 0x67,
		Numpad8 = 0x68,
		Numpad9 = 0x69,

		Multiply = 0x6A,
		Add = 0x6b,
		Separator = 0x6C,
		Subtract = 0x6D,
		Decimal = 0x6E,
		Divide = 0x6F,

		F1 = 0x70,
		F2 = 0x71,
		F3 = 0x72,
		F4 = 0x73,
		F5 = 0x74,
		F6 = 0x75,
		F7 = 0x76,
		F8 = 0x77,
		F9 = 0x78,
		F10 = 0x79,
		F11 = 0x7a,
		F12 = 0x7b,
		F13 = 0x7c,
		F14 = 0x7d,
		F15 = 0x7e,
		F16 = 0x7f,
		F17 = 0x80,
		F18 = 0x81,
		F19 = 0x82,
		F20 = 0x83,
		F21 = 0x84,
		F22 = 0x85,
		F23 = 0x86,
		F24 = 0x87,

		Numlock = 0x90,
		Scroll = 0x91,

		LShift = 0xA0,
		Rshift = 0xA1,
		Lcontrol = 0xA2,
		Rcontrol = 0xA3,
		Lalt = 0xa4,
		Ralt = 0xa5,

		Plus = 0xbb, // '+'
		Comma = 0xbC, // ','
		Minus = 0xbD, // '-'
		Period = 0xBE, // '.'

		exponent = 0xDC, // '^'

		Attn = 0xf6,
		Crsel = 0xf7,
		Exsel = 0xf8,
		Ereof = 0xf9,
		Play = 0xfa,
		Zoom = 0xfb,
		Noname = 0xFC,
		Pa1 = 0xfd,
		OemClear = 0xFE,

		Max = 0x100
	};
}