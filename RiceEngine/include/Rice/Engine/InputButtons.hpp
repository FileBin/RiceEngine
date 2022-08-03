#pragma once

#include "../stdafx.hpp"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_mouse.h"
#include "SDL2/SDL_scancode.h"

NSP_ENGINE_BEGIN

enum class KeyCode {
	MouseLeft = SDL_BUTTON_LEFT, // Left mouse button
	MouseRight = SDL_BUTTON_RIGHT, // Right mouse button
	Cancel = SDLK_CANCEL, // Control-break processing
	MouseMiddle = SDL_BUTTON_MIDDLE, // Middle mouse button (three-button mouse)
	XButton1 = SDL_BUTTON_X1, // X1 mouse button
	XButton2 = SDL_BUTTON_X2, // X2 mouse button
	Backspace = SDLK_BACKSPACE, // BACKSPACE key
	Tab = SDLK_TAB, // TAB key
	Clear = SDLK_CLEAR, // CLEAR key
	Return = SDLK_RETURN, // ENTER key
	Shift = SDLK_LSHIFT, // SHIFT key
	Control = SDLK_LCTRL, // CTRL key
	Alt = SDLK_LALT, // ALT key
	Pause = SDLK_PAUSE, // PAUSE key
	Capital = SDLK_CAPSLOCK, // CAPS LOCK key

	Escape = SDLK_ESCAPE,
    Space = SDLK_SPACE,
    PageUp = SDLK_PAGEUP,
    PageDown = SDLK_PAGEDOWN,
    End = SDLK_END,
    Home = SDLK_HOME,
    Left = SDLK_LEFT, // left arrow key
    Up = SDLK_UP, // up arrow key
    Right = SDLK_RIGHT, // right arrow key
    Down = SDLK_DOWN, // down arrow key
    Select = SDLK_SELECT, // select key
    Execute = SDLK_EXECUTE,
    Snapshot = SDLK_PRINTSCREEN, // print screen key
    Insert = SDLK_INSERT, // insert key
    Delete = SDLK_DELETE, // delete key
    Help = SDLK_HELP, // help key

    Button0 = SDLK_0, // 0 key
    Button1 = SDLK_1, // 1 key
    Button2 = SDLK_2, // 2 key
    Button3 = SDLK_3, // 3 key
    Button4 = SDLK_4, // 4 key
    Button5 = SDLK_5, // 5 key
    Button6 = SDLK_6, // 6 key
    Button7 = SDLK_7, // 7 key
    Button8 = SDLK_8, // 8 key
    Button9 = SDLK_9, // 9 key
    A = SDLK_a, // A key
    B = SDLK_b, // B key
    C = SDLK_c, // C key
    D = SDLK_d, // D key
    E = SDLK_e, // E key
    F = SDLK_f, // F key
    G = SDLK_g, // G key
    H = SDLK_h, // H key
    I = SDLK_i, // I key
    J = SDLK_j, // J key
    K = SDLK_k, // K key
    L = SDLK_l, // L key
    M = SDLK_m, // M key
    N = SDLK_n, // N key
    O = SDLK_o, // O key
    P = SDLK_p, // P key
    Q = SDLK_q, // Q key
    R = SDLK_r, // R key
    S = SDLK_s, // S key
    T = SDLK_t, // T key
    U = SDLK_u, // U key
    V = SDLK_v, // V key
    W = SDLK_w, // W key
    X = SDLK_x, // X key
    Y = SDLK_y, // Y key
    Z = SDLK_z, // Z key

    LeftWindows = SDLK_LGUI, // Left Windows key (Natural keyboard)
    RightWindows = SDLK_RGUI, // Right Windows key (Natural keyboard)
    Apps = SDLK_APPLICATION, // Applications key (Natural keyboard)
    Sleep = SDLK_SLEEP, // Computer Sleep key

    Num0 = SDLK_KP_0, // Numeric keypad 0 key
    Num1 = SDLK_KP_1, // Numeric keypad 1 key
    Num2 = SDLK_KP_2, // Numeric keypad 2 key
    Num3 = SDLK_KP_3, // Numeric keypad 3 key
    Num4 = SDLK_KP_4, // Numeric keypad 4 key
    Num5 = SDLK_KP_5, // Numeric keypad 5 key
    Num6 = SDLK_KP_6, // Numeric keypad 6 key
    Num7 = SDLK_KP_7, // Numeric keypad 7 key
    Num8 = SDLK_KP_8, // Numeric keypad 8 key
    Num9 = SDLK_KP_9, // Numeric keypad 9 key
    NumMultiply = SDLK_KP_MULTIPLY, // Multiply key
    NumAdd = SDLK_KP_PLUS, // Add key
    NumSubtract = SDLK_KP_MINUS, // Subtract key
    NumDecimal = SDLK_KP_PERIOD, // Decimal point key
    NumDivide = SDLK_KP_DIVIDE, // Divide key
    NumEnter = SDLK_KP_ENTER, // Enter key
    NumEqual = SDLK_KP_EQUALS, // Equals key
    NumClear = SDLK_KP_CLEAR, // Clear key
    NumInsert = SDLK_INSERT, // Insert key
    NumDelete = SDLK_DELETE, // Delete key
    NumHome = SDLK_HOME, // Home key
    NumEnd = SDLK_END, // End key
    NumPageUp = SDLK_PAGEUP, // Page up key
    NumPageDown = SDLK_PAGEDOWN, // Page down key
    NumUp = SDLK_UP, // Up arrow key
    NumDown = SDLK_DOWN, // Down arrow key
    NumLeft = SDLK_LEFT, // Left arrow key
    NumRight = SDLK_RIGHT, // Right arrow key


    F1 = SDLK_F1, // F1 key
    F2 = SDLK_F2, // F2 key
    F3 = SDLK_F3, // F3 key
    F4 = SDLK_F4, // F4 key
    F5 = SDLK_F5, // F5 key
    F6 = SDLK_F6, // F6 key
    F7 = SDLK_F7, // F7 key
    F8 = SDLK_F8, // F8 key
    F9 = SDLK_F9, // F9 key
    F10 = SDLK_F10, // F10 key
    F11 = SDLK_F11, // F11 key
    F12 = SDLK_F12, // F12 key
    F13 = SDLK_F13, // F13 key
    F14 = SDLK_F14, // F14 key
    F15 = SDLK_F15, // F15 key
    F16 = SDLK_F16, // F16 key
    F17 = SDLK_F17, // F17 key
    F18 = SDLK_F18, // F18 key
    F19 = SDLK_F19, // F19 key
    F20 = SDLK_F20, // F20 key
    F21 = SDLK_F21, // F21 key
    F22 = SDLK_F22, // F22 key
    F23 = SDLK_F23, // F23 key
    F24 = SDLK_F24, // F24 key

    NumLock = SDLK_NUMLOCKCLEAR, // Numeric keypad NumLock key
    ScrollLock = SDLK_SCROLLLOCK, // Scroll Lock key

    LeftShift = SDLK_LSHIFT, // Left Shift key
    RightShift = SDLK_RSHIFT, // Right Shift key
    LeftControl = SDLK_LCTRL, // Left Control key
    RightControl = SDLK_RCTRL, // Right Control key
    LeftAlt = SDLK_LALT, // Left Alt key
    RightAlt = SDLK_RALT, // Right Alt key


    Plus = SDLK_PLUS, // Plus key
    Minus = SDLK_MINUS, // Minus key
    Period = SDLK_PERIOD, // Period key
    Comma = SDLK_COMMA, // Comma key
    Slash = SDLK_SLASH, // Slash key
    Backslash = SDLK_BACKSLASH, // Back slash key
    Semicolon = SDLK_SEMICOLON, // Semicolon key
    Quote = SDLK_QUOTE, // Quote key
    Tilde = SDLK_BACKQUOTE, // Tilde key
    Equal = SDLK_EQUALS, // Equal key
    LeftBracket = SDLK_LEFTBRACKET, // Left bracket key
    RightBracket = SDLK_RIGHTBRACKET, // Right bracket key
};

#undef KEY_CODE

NSP_ENGINE_END
