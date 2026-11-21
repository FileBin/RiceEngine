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

    /**
     * Writes a block straight into this buffer's host-visible memory instead of
     * queueing a staging copy (which Buffer::setData does, one staging slot per
     * call, submitted at the next draw).
     *
     * For a value that changes every frame - a clock the shader animates with -
     * that staging round trip is neither necessary nor what the caller wants:
     * the buffer is already host visible, so the write is a memcpy into a
     * mapping that is created once (on the first call) and kept for the life of
     * the buffer. Mapping/unmapping per call is not an option: on a software
     * driver each vkMapMemory synchronises with the device, which cost roughly
     * half a frame per call in the headless (lavapipe) test runs.
     *
     * Synchronisation assumption: call it between frames, from the game's
     * update step, before the frame's command buffers are submitted. The host
     * write is then flushed to device-visible memory before the draw that reads
     * it (skipped when the buffer's memory turned out to be host coherent). Calling it while a submitted frame may still read the block is legal
     * but may tear: the GPU could see half of the new value. There is no
     * per-frame-in-flight copy, so a value written for frame N is what every
     * later submission reads - exactly what a clock wants.
     *
     * `size` is clamped to the buffer's allocated size; a larger block is
     * truncated rather than corrupting the next allocation.
     */
    void updateData(const void *data, size_t size);

    bool isAllocated();

    void cleanup() override;

  private:
    friend class CommandBuffer_API_data;
    wptr<Shader> shader;
    // Persistent mapping created by updateData(); released in cleanup() before
    // the buffer memory goes away.
    void *mapped = nullptr;

    void unmap();
};

NSP_GL_END
