/*
 * VertexBuffer.cpp
 *
 *  Created on: Apr 29, 2022
 *      Author: root
 */

#include "pch.h"

#include <Rice/GL/VertexBuffer.hpp>
#include <cstring>

#include "Vulkan_API_code/api_Buffer.hpp"

NSP_GL_BEGIN

VertexBuffer::VertexBuffer(ptr<GraphicsManager> g_mgr, VertexList &initialData)
    : Buffer(g_mgr), stride(0), current_count(0) {
    reset(initialData);
}

bool VertexBuffer::isAllocated() { return stride != 0; }

void VertexBuffer::cleanup() {
    if (isAllocated()) {
        api_data->free();
    }
}

void VertexBuffer::reset(VertexList &initialData) {
    cleanup();

    uint s = initialData.getStride();
    uint n = initialData.count();

    if (n != 0) {
        current_count = n;
        stride = s;

        allocate(s * n, BufferUsage::Vertex);
        char *buffer = new char[n * s];
        for (uint i = 0; i < n; ++i) {
            memcpy(buffer + s * i, initialData.getData(i), s);
        }
        setData(buffer, s * n);
        delete[] buffer;
    }
}

void VertexBuffer::updateVertices(VertexList &vert, uint start) {
    uint s = vert.getStride();
    if (s != stride)
        THROW_EXCEPTION("Incorrect vertex input!");

    uint n = vert.count();

    char *buffer = new char[n * s];
    for (uint i = 0; i < n; ++i) {
        memcpy(buffer + s * i, vert.getData(i), s);
    }
    setData(buffer, s * n, start);
    delete[] buffer;
}

NSP_GL_END
