/*
 * UniformBuffer.hpp
 *
 *  Created on: 25 июн. 2022 г.
 *      Author: filebin
 */

#include "stdafx.hpp"

NSP_GL_BEGIN

class UniformBuffer;

struct UniformBuffer_API_Data;

NSP_GL_END

#pragma once

#include "Buffer.hpp"
#include "Shader.hpp"

NSP_GL_BEGIN

class UniformBuffer : public Buffer {
  public:
    UniformBuffer(ptr<GraphicsManager> g_mgr, uint size);
    void reset(uint size);

    template <typename T> void updateData(const T &data) {
        Buffer::setData((void *)&data, sizeof(T));
    }

    bool isAllocated();

    void cleanup() override;

  private:
    friend class CommandBuffer_API_data;
    wptr<Shader> shader;
};

NSP_GL_END
