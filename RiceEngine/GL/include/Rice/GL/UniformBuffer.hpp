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

	void setShader(ptr<Shader> shader, uint binding);

	template<typename T>
	void updateData(const T& data) {
		Buffer::setData((void*)&data, sizeof(T));
	}

	template<typename T>
	T getData() {
		T data;
		Buffer::getData((void*)&data, sizeof(T));
		return data;
	}

	bool isAllocated();

	void cleanup() override;
private:
	friend class CommandBuffer_API_data;
    wptr<Shader> shader;
};

NSP_GL_END
