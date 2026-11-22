/*
 * Buffer.hpp
 *
 *  Created on: 26 April 2022
 *      Author: FileBinsLapTop
 */

#include "stdafx.hpp"

NSP_GL_BEGIN

class Buffer;

enum class BufferUsage { Vertex, Index, Uniform, Storage };

/*
 * Extra usages a StorageBuffer asks for on top of eStorageBuffer.
 *
 * A compute-written buffer is usually more than an SSBO: the GPU mesher writes
 * vertex and index data that the same frame then draws, and it writes the
 * VkDrawIndexedIndirectCommand records it is drawn with
 * (docs/tetrahedral-mesh.md §6.3), so a single buffer has to carry several
 * fixed-function roles at once. `VertexIndex` and `VertexIndexIndirect` are
 * those combinations; plain Storage is the SSBO a compute shader only reads.
 */
enum class StorageUsage {
    Storage = 0,
    Vertex = 1,
    Index = 2,
    Indirect = 4,
    VertexIndex = 3,
    VertexIndexIndirect = 7,
};

enum class IndexBufferType { UInt16, UInt32 };

struct Buffer_API_Data;

NSP_GL_END

#include "GraphicsComponentBase.hpp"
#include "GraphicsManager.hpp"

#pragma once

NSP_GL_BEGIN

class Buffer : public GraphicsComponentBase {
  protected:
    friend struct CommandBuffer_API_data;
    friend struct DescriptorSetCreator;
    uptr<Buffer_API_Data> api_data;
    Buffer(ptr<GraphicsManager> g_mgr);
    ~Buffer() override;

  public:
    Buffer(ptr<GraphicsManager> g_mgr, BufferUsage usage, size_t size);
    Buffer(ptr<GraphicsManager> g_mgr, BufferUsage usage, data_t initialData);

    void cleanup() override;

  protected:
    void build_api();
    void allocate(size_t size, BufferUsage usage);
    void setData(void *pSrc, size_t nData, size_t offset = 0);
};

NSP_GL_END
