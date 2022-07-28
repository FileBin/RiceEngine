/*
 * HelloWorld.hpp
 *
 *  Created on: 31 March 2022
 *      Author: FileBinsLapTop
 */

#pragma once

#include "../stdafx.hpp"
#include "../Engine/Window.hpp"

NSP_TESTS_BEGIN

static void HelloWindow() {
	using namespace Rice;
	Window win;
	win.create({ "Vulkan" });

	while(win.update());
}

NSP_TESTS_END
