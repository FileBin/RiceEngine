#pragma once
#include "stdafx.h"
#include "InputManager.h"

namespace Game {
	struct DescWindow {
		DescWindow() :
			caption(L"Game"),
			width(640),
			height(480),
			posx(200),
			posy(20),
			resizing(true) {}

		int posx;
		int posy;
		std::wstring caption;	///< ��������� ����
		int width;				///< ������ ���������� ����� ����
		int height;				///< ������ ���������� ����� ����
		bool resizing;
	};

	class Window {
	public:
		Window();

		static Window* Get() { return wndthis; }

		// ������� ����
		bool Create(const DescWindow& desc);

		// ��������� ������� ����
		void RunEvent();

		// ������� ����.
		void Close();

		void SetInputMgr(InputManager* inputmgr);

		HWND GetHWND() const { return m_hwnd; }
		int GetLeft() const { return desc.posx; }
		int GetTop() const { return desc.posy; }
		int GetWidth() const { return desc.width; }
		int GetHeight() const { return desc.height; }
		// ������� ��������� ����
		const std::wstring& GetCaption() const { return desc.caption; }

		// ��������, ���� �� ��������� � ������
		bool IsExit() const { return m_isexit; }
		// �������� �� ���������� ����
		bool IsActive() const { return m_active; }
		// �������� �� ��������� ����
		// ��������������: ����� ������ ��������� ���� �� ��������� �������
		bool IsResize() {
			bool ret = m_isresize;
			m_isresize = false;
			return ret;
		}

		// ��������� �������
		LRESULT WndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
		InputManager* inputmgr;
	private:
		void m_UpdateWindowState();

		static Window* wndthis;

		DescWindow desc;	// �������� ����
		HWND m_hwnd;		// ���������� ����	
		bool m_isexit;		// ���� ���������� � ������� ������	
		bool m_active;		// ���� �������?
		bool m_minimized;
		bool m_maximized;
		bool m_isresize;	// ���� ���� �������� ������
	};

	// ��������� �������
	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
}