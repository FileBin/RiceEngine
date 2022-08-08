#pragma once

#include "SDL2/SDL_mouse.h"
#include "SDL2/SDL_scancode.h"

NSP_ENGINE_BEGIN

enum class KeyCode {
	MouseLeft = SDL_BUTTON_LEFT, // Left mouse button
	MouseRight = SDL_BUTTON_RIGHT, // Right mouse button
	Cancel = SDL_SCANCODE_CANCEL, // Control-break processing
	MouseMiddle = SDL_BUTTON_MIDDLE, // Middle mouse button (three-button mouse)
	XButton1 = SDL_BUTTON_X1, // X1 mouse button
	XButton2 = SDL_BUTTON_X2, // X2 mouse button
	Backspace = SDL_SCANCODE_BACKSPACE, // BACKSPACE key
	Tab = SDL_SCANCODE_TAB, // TAB key
	Clear = SDL_SCANCODE_CLEAR, // CLEAR key
	Return = SDL_SCANCODE_RETURN, // ENTER key
	Shift = SDL_SCANCODE_LSHIFT, // SHIFT key
	Control = SDL_SCANCODE_LCTRL, // CTRL key
	Alt = SDL_SCANCODE_LALT, // ALT key
	Pause = SDL_SCANCODE_PAUSE, // PAUSE key
	Capital = SDL_SCANCODE_CAPSLOCK, // CAPS LOCK key

	Escape = SDL_SCANCODE_ESCAPE,
    Space = SDL_SCANCODE_SPACE,
    PageUp = SDL_SCANCODE_PAGEUP,
    PageDown = SDL_SCANCODE_PAGEDOWN,
    End = SDL_SCANCODE_END,
    Home = SDL_SCANCODE_HOME,
    Left = SDL_SCANCODE_LEFT, // left arrow key
    Up = SDL_SCANCODE_UP, // up arrow key
    Right = SDL_SCANCODE_RIGHT, // right arrow key
    Down = SDL_SCANCODE_DOWN, // down arrow key
    Select = SDL_SCANCODE_SELECT, // select key
    Execute = SDL_SCANCODE_EXECUTE,
    Snapshot = SDL_SCANCODE_PRINTSCREEN, // print screen key
    Insert = SDL_SCANCODE_INSERT, // insert key
    Delete = SDL_SCANCODE_DELETE, // delete key
    Help = SDL_SCANCODE_HELP, // help key

    Button0 = SDL_SCANCODE_0, // 0 key
    Button1 = SDL_SCANCODE_1, // 1 key
    Button2 = SDL_SCANCODE_2, // 2 key
    Button3 = SDL_SCANCODE_3, // 3 key
    Button4 = SDL_SCANCODE_4, // 4 key
    Button5 = SDL_SCANCODE_5, // 5 key
    Button6 = SDL_SCANCODE_6, // 6 key
    Button7 = SDL_SCANCODE_7, // 7 key
    Button8 = SDL_SCANCODE_8, // 8 key
    Button9 = SDL_SCANCODE_9, // 9 key
    A = SDL_SCANCODE_A, // A key
    B = SDL_SCANCODE_B, // B key
    C = SDL_SCANCODE_C, // C key
    D = SDL_SCANCODE_D, // D key
    E = SDL_SCANCODE_E, // E key
    F = SDL_SCANCODE_F, // F key
    G = SDL_SCANCODE_G, // G key
    H = SDL_SCANCODE_H, // H key
    I = SDL_SCANCODE_I, // I key
    J = SDL_SCANCODE_J, // J key
    K = SDL_SCANCODE_K, // K key
    L = SDL_SCANCODE_L, // L key
    M = SDL_SCANCODE_M, // M key
    N = SDL_SCANCODE_N, // N key
    O = SDL_SCANCODE_O, // O key
    P = SDL_SCANCODE_P, // P key
    Q = SDL_SCANCODE_Q, // Q key
    R = SDL_SCANCODE_R, // R key
    S = SDL_SCANCODE_S, // S key
    T = SDL_SCANCODE_T, // T key
    U = SDL_SCANCODE_U, // U key
    V = SDL_SCANCODE_V, // V key
    W = SDL_SCANCODE_W, // W key
    X = SDL_SCANCODE_X, // X key
    Y = SDL_SCANCODE_Y, // Y key
    Z = SDL_SCANCODE_Z, // Z key

    LeftWindows = SDL_SCANCODE_LGUI, // Left Windows key (Natural keyboard)
    RightWindows = SDL_SCANCODE_RGUI, // Right Windows key (Natural keyboard)
    Apps = SDL_SCANCODE_APPLICATION, // Applications key (Natural keyboard)
    Sleep = SDL_SCANCODE_SLEEP, // Computer Sleep key

    Num0 = SDL_SCANCODE_KP_0, // Numeric keypad 0 key
    Num1 = SDL_SCANCODE_KP_1, // Numeric keypad 1 key
    Num2 = SDL_SCANCODE_KP_2, // Numeric keypad 2 key
    Num3 = SDL_SCANCODE_KP_3, // Numeric keypad 3 key
    Num4 = SDL_SCANCODE_KP_4, // Numeric keypad 4 key
    Num5 = SDL_SCANCODE_KP_5, // Numeric keypad 5 key
    Num6 = SDL_SCANCODE_KP_6, // Numeric keypad 6 key
    Num7 = SDL_SCANCODE_KP_7, // Numeric keypad 7 key
    Num8 = SDL_SCANCODE_KP_8, // Numeric keypad 8 key
    Num9 = SDL_SCANCODE_KP_9, // Numeric keypad 9 key
    NumMultiply = SDL_SCANCODE_KP_MULTIPLY, // Multiply key
    NumAdd = SDL_SCANCODE_KP_PLUS, // Add key
    NumSubtract = SDL_SCANCODE_KP_MINUS, // Subtract key
    NumDecimal = SDL_SCANCODE_KP_PERIOD, // Decimal point key
    NumDivide = SDL_SCANCODE_KP_DIVIDE, // Divide key
    NumEnter = SDL_SCANCODE_KP_ENTER, // Enter key
    NumEqual = SDL_SCANCODE_KP_EQUALS, // Equals key
    NumClear = SDL_SCANCODE_KP_CLEAR, // Clear key
    NumInsert = SDL_SCANCODE_INSERT, // Insert key
    NumDelete = SDL_SCANCODE_DELETE, // Delete key
    NumHome = SDL_SCANCODE_HOME, // Home key
    NumEnd = SDL_SCANCODE_END, // End key
    NumPageUp = SDL_SCANCODE_PAGEUP, // Page up key
    NumPageDown = SDL_SCANCODE_PAGEDOWN, // Page down key
    NumUp = SDL_SCANCODE_UP, // Up arrow key
    NumDown = SDL_SCANCODE_DOWN, // Down arrow key
    NumLeft = SDL_SCANCODE_LEFT, // Left arrow key
    NumRight = SDL_SCANCODE_RIGHT, // Right arrow key


    F1 = SDL_SCANCODE_F1, // F1 key
    F2 = SDL_SCANCODE_F2, // F2 key
    F3 = SDL_SCANCODE_F3, // F3 key
    F4 = SDL_SCANCODE_F4, // F4 key
    F5 = SDL_SCANCODE_F5, // F5 key
    F6 = SDL_SCANCODE_F6, // F6 key
    F7 = SDL_SCANCODE_F7, // F7 key
    F8 = SDL_SCANCODE_F8, // F8 key
    F9 = SDL_SCANCODE_F9, // F9 key
    F10 = SDL_SCANCODE_F10, // F10 key
    F11 = SDL_SCANCODE_F11, // F11 key
    F12 = SDL_SCANCODE_F12, // F12 key
    F13 = SDL_SCANCODE_F13, // F13 key
    F14 = SDL_SCANCODE_F14, // F14 key
    F15 = SDL_SCANCODE_F15, // F15 key
    F16 = SDL_SCANCODE_F16, // F16 key
    F17 = SDL_SCANCODE_F17, // F17 key
    F18 = SDL_SCANCODE_F18, // F18 key
    F19 = SDL_SCANCODE_F19, // F19 key
    F20 = SDL_SCANCODE_F20, // F20 key
    F21 = SDL_SCANCODE_F21, // F21 key
    F22 = SDL_SCANCODE_F22, // F22 key
    F23 = SDL_SCANCODE_F23, // F23 key
    F24 = SDL_SCANCODE_F24, // F24 key

    NumLock = SDL_SCANCODE_NUMLOCKCLEAR, // Numeric keypad NumLock key
    ScrollLock = SDL_SCANCODE_SCROLLLOCK, // Scroll Lock key

    LeftShift = SDL_SCANCODE_LSHIFT, // Left Shift key
    RightShift = SDL_SCANCODE_RSHIFT, // Right Shift key
    LeftControl = SDL_SCANCODE_LCTRL, // Left Control key
    RightControl = SDL_SCANCODE_RCTRL, // Right Control key
    LeftAlt = SDL_SCANCODE_LALT, // Left Alt key
    RightAlt = SDL_SCANCODE_RALT, // Right Alt key


    Plus = SDL_SCANCODE_KP_PLUS, // Plus key
    Minus = SDL_SCANCODE_MINUS, // Minus key
    Period = SDL_SCANCODE_PERIOD, // Period key
    Comma = SDL_SCANCODE_COMMA, // Comma key
    Slash = SDL_SCANCODE_SLASH, // Slash key
    Backslash = SDL_SCANCODE_BACKSLASH, // Back slash key
    Semicolon = SDL_SCANCODE_SEMICOLON, // Semicolon key
    Quote = SDL_SCANCODE_APOSTROPHE, // Quote key
    Tilde = SDL_SCANCODE_GRAVE, // Tilde key
    Equal = SDL_SCANCODE_EQUALS, // Equal key
    LeftBracket = SDL_SCANCODE_LEFTBRACKET, // Left bracket key
    RightBracket = SDL_SCANCODE_RIGHTBRACKET, // Right bracket key
};

#undef KEY_CODE

NSP_ENGINE_END
