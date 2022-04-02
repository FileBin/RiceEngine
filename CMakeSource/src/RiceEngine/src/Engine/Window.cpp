#include "pch.h"

#include <Rice/Engine/Window.hpp>
#include <Rice/Engine/Log.hpp>

NSP_ENGINE_BEGIN

pWindow Window::instance = nullptr;

Window::Window() :
		inputmgr(nullptr), handle(nullptr), is_exit(false), is_active(true), is_resizing(
				false) {
	if (instance.isNotNull())
		instance = this;
	else
		Log::log(Log::Error, L"Window construction error");
}

bool Window::create(DescWindow desc) {
	Log::debug("Creating window..");
	this->desc = desc;

	handle = SDL_CreateWindow("VulkanTest", desc.posx, desc.posy, desc.width,
			desc.height, SDL_WINDOW_VULKAN);

	updateWindowState();

	if (handle.isNotNull()) {
		return true;
		Log::debug("Window successfully created!");
	}
	return false;
}

bool Window::update() {
	SDL_Event event;
	SDL_PollEvent(&event);
	handleEvent(event);
	return !is_exit;
}

void Window::cleanup() {
	if (handle.isNotNull())
		SDL_DestroyWindow(handle.get());
	handle = nullptr;
	Log::debug("Window closed!");
}

void Window::handleEvent(SDL_Event& e) {
	is_resizing = false;

	switch (e.type) {
	case SDL_WINDOWEVENT:
		handleWindowEvent(e.window);
		break;
	case SDL_KEYDOWN:
		inputmgr->eventKey((KeyCode)e.key.keysym.sym, true);
		break;
	case SDL_KEYUP:
		inputmgr->eventKey((KeyCode)e.key.keysym.sym, false);
		break;
	case SDL_MOUSEWHEEL:
		inputmgr->eventMouseWheel(e.wheel.y);
	}

	updateWindowState();
}

void Window::handleWindowEvent(SDL_WindowEvent& e) {
	switch (e.event) {
	case SDL_WINDOWEVENT_FOCUS_GAINED:
	case SDL_WINDOWEVENT_SHOWN:
		is_active = true;
		break;
	case SDL_WINDOWEVENT_HIDDEN:
	case SDL_WINDOWEVENT_FOCUS_LOST:
		is_active = false;
		break;
	case SDL_WINDOWEVENT_CLOSE:
		is_exit = true;
		break;
	case SDL_WINDOWEVENT_RESIZED:
		is_resizing = true;
		break;
	case SDL_WINDOWEVENT_MOVED:
		break;
	}
}

void Window::setInputMgr(pInputManager inputmgr) {
	this->inputmgr = inputmgr;
	updateWindowState();
}

void Window::updateWindowState() {
	if (auto rect = SDL_GetWindowMouseRect(handle.get())) {
		desc.posx = rect->x;
		desc.posy = rect->y;
		desc.width = rect->w;
		desc.height = rect->h;
		if (inputmgr.isNotNull())
			inputmgr->setMouseRect(*rect);
	}
}

NSP_ENGINE_END
