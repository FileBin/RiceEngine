/*
 * VertexBuffer.hpp
 *
 *  Created on: Apr 29, 2022
 *      Author: root
 */

#include "stdafx.hpp"

NSP_GL_BEGIN

class VertexBuffer;

NSP_GL_END

#pragma once

#include "Buffer.hpp"
#include "Mesh.hpp"

NSP_GL_BEGIN

class VertexBuffer : public Buffer {
  public:
    VertexBuffer(ptr<GraphicsManager> g_mgr, VertexList &initialData);
    void reset(VertexList &initialData);

    template <typename Vertex> void updateVertice(const Vertex &data, uint index) {
        constexpr uint s = sizeof(Vertex);
        if (s != stride)
            THROW_EXCEPTION("Incorrect vertex input!");

        setData((void *)&data, s, s * index);
    }
    void updateVertices(VertexList &data, uint start_index);

    bool isAllocated();

    void cleanup() override;

  private:
    uint stride;
    uint current_count;
};

NSP_GL_END
