/*
 * HelloWorld.hpp
 *
 *  Created on: 31 March 2022
 *      Author: FileBinsLapTop
 */

#pragma once

#include "Rice/Engine/InputButtons.hpp"
#include "Rice/Engine/Log.hpp"
#include "Rice/Engine/Window.hpp"

NSP_TESTS_BEGIN

static void HelloWindow() {
    using namespace Rice;
    Log::init();
    auto win = Window::create({"Vulkan"});
    auto input = win->getInputManager();
    while (win->isOpen()) {
        if (input->getKey(KeyCode::MouseLeft)) {
            input->lockMouse();
        }
        auto infinitePos = input->getMouseInfinitePos();
        Log::debug("Mouse pos: {}, {}", infinitePos.x, infinitePos.y);
    }
    Log::close();
}

NSP_TESTS_END
