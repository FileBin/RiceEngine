#include "pch.h"
#include <GameEngine/Window.h>
#include <GameEngine/Log.h>

namespace Game {

	Window* Window::wndthis = nullptr;

	Window::Window() :
		inputmgr(nullptr),
		m_hwnd(0),
		m_isexit(false),
		m_active(true),
		m_minimized(false),
		m_maximized(false),
		m_isresize(false) {
		if (!wndthis)
			wndthis = this;
		else
			Log::log(Log::ERR, L"Window construction error");
	}

	bool Window::Create(const DescWindow& desc) {
		inputmgr = InputManager::Init();
		Log::log(Log::Debug, L"Window Create");
		this->desc = desc;

		WNDCLASSEXW wnd;

		wnd.cbSize = sizeof(WNDCLASSEXW);
		wnd.style = CS_HREDRAW | CS_VREDRAW;
		wnd.lpfnWndProc = StaticWndProc;
		wnd.cbClsExtra = 0;
		wnd.cbWndExtra = 0;
		wnd.hInstance = 0;
		wnd.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wnd.hIconSm = wnd.hIcon;
		wnd.hCursor = LoadCursor(0, IDC_ARROW);
		wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wnd.lpszMenuName = NULL;
		wnd.lpszClassName = L"D3D11F";
		wnd.cbSize = sizeof(WNDCLASSEX);

		if (!RegisterClassEx(&wnd)) {
			Log::log(Log::ERR, L"RegisterClassEx failed");
			return false;
		}

		RECT rect = { 0, 0, desc.width, desc.height };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

		long lwidth = rect.right - rect.left;
		long lheight = rect.bottom - rect.top;

		long lleft = (long)desc.posx;
		long ltop = (long)desc.posy;

		m_hwnd = CreateWindowEx(NULL, L"D3D11F", desc.caption.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE, lleft, ltop, lwidth, lheight, NULL, NULL, NULL, NULL);

		if (!m_hwnd) {
			Log::log(Log::ERR, L"CreateWindowEx failed");
			return false;
		}

		ShowWindow(m_hwnd, SW_SHOW);
		UpdateWindow(m_hwnd);

		RAWINPUTDEVICE Rid[2];

		Rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
		Rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
		Rid[0].dwFlags = 0;    // adds mouse and also ignores legacy mouse messages
		Rid[0].hwndTarget = m_hwnd;

		if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE)
		{
			Log::log(Log::ERR, L"Failed");
		//registration failed. Call GetLastError for the cause of the error
		}

		return true;
	}

	void Window::RunEvent() {
		MSG msg;			// ������� ����	
		// ������������� ��� ����������� �������
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Window::Close() {
		if (m_hwnd)
			DestroyWindow(m_hwnd);
		m_hwnd = nullptr;
		Log::log(Log::Debug, L"Window Close");
	}

	LRESULT Window::WndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam) {
		switch (nMsg) {
		case WM_CREATE:
			return 0;
		case WM_CLOSE:
			m_isexit = true;
			return 0;
		case WM_ACTIVATE:
			if (LOWORD(wParam) != WA_INACTIVE)
				m_active = true;
			else
				m_active = false;
			return 0;
		case WM_MOVE:
			desc.posx = LOWORD(lParam);
			desc.posy = HIWORD(lParam);
			m_UpdateWindowState();
			return 0;
		case WM_SIZE:
			if (!desc.resizing)
				return 0;
			desc.width = LOWORD(lParam);
			desc.height = HIWORD(lParam);
			m_isresize = true;
			if (wParam == SIZE_MINIMIZED) {
				m_active = false;
				m_minimized = true;
				m_maximized = false;
			} else if (wParam == SIZE_MAXIMIZED) {
				m_active = true;
				m_minimized = false;
				m_maximized = true;
			} else if (wParam == SIZE_RESTORED) {
				if (m_minimized) {
					m_active = true;
					m_minimized = false;
				} else if (m_maximized) {
					m_active = true;
					m_maximized = false;
				}
			}
			m_UpdateWindowState();
			return 0;
		case WM_INPUT:
		case WM_MOUSEMOVE: case WM_LBUTTONUP: case WM_LBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDOWN: case WM_MOUSEWHEEL: case WM_KEYDOWN: case WM_KEYUP:
			if (inputmgr)
				inputmgr->UpdateWindow(nMsg, wParam, lParam);
			return 0;
		}

		return DefWindowProcW(hwnd, nMsg, wParam, lParam);
	}

	void Window::SetInputMgr(InputManager* inputmgr) {
		this->inputmgr = inputmgr;
		m_UpdateWindowState();
	}

	void Window::m_UpdateWindowState() {
		RECT ClientRect;
		ClientRect.left = desc.posx;
		ClientRect.top = desc.posy;
		ClientRect.right = desc.width;
		ClientRect.bottom = desc.height;
		if (inputmgr)
			inputmgr->SetRect(ClientRect);
	}

	LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam) {
		return Window::Get()->WndProc(hwnd, nMsg, wParam, lParam);
	}
}