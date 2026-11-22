/*
 * StorageBuffer.hpp — a shader storage buffer (SSBO) the CPU can also read.
 *
 * This is the engine's compute-side resource: a buffer created with
 * eStorageBuffer, host visible, so a shader can write it and the CPU can map
 * and read it back without a staging copy. It is also the only readback path
 * in the engine (docs/tetrahedral-mesh.md §6.1): a self-test dispatches a
 * compute shader, waits for the frame (GraphicsManager::sync) and compares the
 * bytes here.
 *
 * `usage` adds the fixed-function roles the buffer has in the draw, because the
 * GPU mesher writes vertex/index data and the indirect draw records itself
 * (§6.3) - one buffer, several usages. Use StorageUsage::Storage for a plain
 * SSBO.
 *
 * The mapping is created on first use and kept. Call write()/read() between
 * frames: a write is a memcpy into device-visible memory (flushed when the
 * memory type is not host coherent), and a read is only valid after the GPU
 * work that wrote the buffer has been waited on.
 */

#include "stdafx.hpp"

NSP_GL_BEGIN

class StorageBuffer;

NSP_GL_END

#pragma once

#include "Buffer.hpp"

NSP_GL_BEGIN

class StorageBuffer : public Buffer {
  public:
    StorageBuffer(ptr<GraphicsManager> g_mgr, size_t size,
                  StorageUsage usage = StorageUsage::Storage);
    ~StorageBuffer() override;

    size_t size() const;

    /* The persistent host mapping of the whole buffer, created on first use. */
    void *map();
    void unmap();

    /* Host -> GPU: memcpy `bytes` at `offset` into the mapping (+ flush when
     * the memory is not host coherent). Truncated at the buffer end. */
    void write(const void *src, size_t bytes, size_t offset = 0);

    /* GPU -> host: memcpy `bytes` at `offset` out of the mapping. Only valid
     * after GraphicsManager::sync() has waited for the frame that wrote it. */
    void read(void *dst, size_t bytes, size_t offset = 0);

    void cleanup() override;

  private:
    void *mapped = nullptr;
};

NSP_GL_END
