/*
 * file: GraphicsQueue.hpp
 * author: filebin
 * date: 2022-10-24 18:00:30
 */

#include "stdafx.hpp"
#include <vector>

NSP_GL_BEGIN
class GraphicsQueue;
NSP_GL_END

#pragma once

#include "Rice/GL/CommandBuffer.hpp"

NSP_GL_BEGIN

class GraphicsQueue {
  public:
    GraphicsQueue();
    ~GraphicsQueue();

    CommandBuffer buffer;
    vec<Shader> shaders;

  private:
};

NSP_GL_END