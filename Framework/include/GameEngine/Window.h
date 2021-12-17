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
		std::wstring caption;	///< заголовок окна
		int width;				///< ширина клиентской части окна
		int height;				///< высота клиентской части окна
		bool resizing;
	};

	class Window {
	public:
		Window();

		static Window* Get() { return wndthis; }

		// Создать окно
		bool Create(const DescWindow& desc);

		// Обработка событий окна
		void RunEvent();

		// Закрыть окно.
		void Close();

		void SetInputMgr(InputManager* inputmgr);

		HWND GetHWND() const { return m_hwnd; }
		int GetLeft() const { return desc.posx; }
		int GetTop() const { return desc.posy; }
		int GetWidth() const { return desc.width; }
		int GetHeight() const { return desc.height; }
		// Вернуть заголовок окна
		const std::wstring& GetCaption() const { return desc.caption; }

		// сообщает, было ли сообщение о выходе
		bool IsExit() const { return m_isexit; }
		// сообщает об активности окна
		bool IsActive() const { return m_active; }
		// сообщает об изменении окна
		// предупреждение: после вызова оповещает окно об обработке события
		bool IsResize() {
			bool ret = m_isresize;
			m_isresize = false;
			return ret;
		}

		// обработка событий
		LRESULT WndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
		InputManager* inputmgr;
	private:
		void m_UpdateWindowState();

		static Window* wndthis;

		DescWindow desc;	// описание окна
		HWND m_hwnd;		// дескриптор окна	
		bool m_isexit;		// флаг сообщающий о событии выхода	
		bool m_active;		// окно активно?
		bool m_minimized;
		bool m_maximized;
		bool m_isresize;	// если окно изменило размер
	};

	// обработка событий
	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
}