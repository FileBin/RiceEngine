#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class Window;
typedef SmartPtr<Window> pWindow;

NSP_ENGINE_END

#pragma once
#include "InputManager.hpp"

NSP_ENGINE_BEGIN

struct DescWindow {
	String caption { L"Game" };
	int width { 640 };
	int height { 480 };
	bool resizing { true };
	int posx { SDL_WINDOWPOS_UNDEFINED };
	int posy { SDL_WINDOWPOS_UNDEFINED };
};

class Window : public ICleanable {
public:
	typedef Event<pWindow> ResizeEvent;

	Window();
	~Window() {	cleanup(); }

	Window(const Window& other) = delete; //no copying

	static const pWindow getInst() { return instance; }

	bool create(DescWindow desc);
	bool update();
	void close() { cleanup(); }
	void cleanup() override;
	void setInputMgr(pInputManager inputmgr);

	const WindowHandle getHandle() const { return handle; }
	int getLeft() const { return desc.posx; }
	int getTop() const { return desc.posy; }
	int getWidth() const {
		int x,y;
		SDL_GetWindowSize(handle.get(), &x, &y);
		return x;
	}
	int getHeight() const {
		int x,y;
		SDL_GetWindowSize(handle.get(), &x, &y);
		return y;
	}

	Vector2i getSize() const {
		Vector2i size;
		SDL_GetWindowSize(handle.get(), (int*)&size.x, (int*)&size.y);
		return size;
	}

	String getCaption() const { return desc.caption; }
	bool isExit() const { return is_exit; }
	bool isActive() const { return is_active; }
	bool isResize() const;
	bool isMinimized() const;
	void updateWindowState();

private:
	void handleEvent(SDL_Event& e);
	void handleWindowEvent(SDL_WindowEvent& e);

public:
	ResizeEvent resize_event;

private:
	DescWindow desc;
	WindowHandle handle;
	bool is_exit;
	bool is_active;
	pInputManager inputmgr;

	static pWindow instance;
};

NSP_ENGINE_END
