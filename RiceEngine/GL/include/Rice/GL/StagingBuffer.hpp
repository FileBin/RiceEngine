/*
 * file: StagingBuffer.hpp
 * author: filebin
 * date: 2022-08-13 18:47:32
 */

#include "Rice/GL/Buffer.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "stdafx.hpp"

NSP_GL_BEGIN
class StagingBuffer;
struct Buffer_API_Data;
NSP_GL_END

#pragma once

#include "GraphicsComponentBase.hpp"
NSP_GL_BEGIN

class StagingBuffer : public GraphicsComponentBase {
  public:
    StagingBuffer(ptr<GraphicsManager> g_mgr, ptr<Buffer> dst, uint nData,
                  void *pData);
    template <typename T>
    StagingBuffer(ptr<GraphicsManager> g_mgr, ptr<Buffer> dst, T &data)
        : StagingBuffer(g_mgr, dst, sizeof(data), &data) {}
    ~StagingBuffer();

  private:
    void setData(void *pSrc, size_t nData, size_t offset = 0);

    uptr<Buffer_API_Data> api_data;
};

NSP_GL_END