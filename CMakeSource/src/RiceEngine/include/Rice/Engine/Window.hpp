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
	int getWidth() const { return desc.width; }
	int getHeight() const { return desc.height; }

	String getCaption() const { return desc.caption; }
	bool isExit() const { return is_exit; }
	bool isActive() const { return is_active; }
	bool isResize() const { return is_resizing; }

private:
	void updateWindowState();
	void handleEvent(SDL_Event& e);
	void handleWindowEvent(SDL_WindowEvent& e);

	static pWindow instance;

	DescWindow desc;
	WindowHandle handle;
	bool is_exit;
	bool is_active;
	bool is_resizing;
	pInputManager inputmgr;
};

NSP_ENGINE_END
