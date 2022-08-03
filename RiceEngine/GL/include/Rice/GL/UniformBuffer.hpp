/*
 * UniformBuffer.hpp
 *
 *  Created on: 25 июн. 2022 г.
 *      Author: filebin
 */

#include "Rice/Util/Event.hpp"
#include "stdafx.hpp"

NSP_GL_BEGIN

class UniformBuffer;

struct UniformBuffer_API_Data;

NSP_GL_END

#pragma once

#include "Buffer.hpp"
#include "Shader.hpp"

NSP_GL_BEGIN

class UniformBuffer : public GraphicsComponentBase {
public:
	UniformBuffer(ptr<GraphicsManager> g_mgr, uint size);
    ~UniformBuffer() override;
	void reset(uint size);

	void setShader(ptr<Shader> shader);
	void setBinding(uint binding, uint size);

	template<typename T>
	void updateData(const T& data) {
		updateData((void*)&data, sizeof(T));
	}

	template<typename T>
	void updateDataAll(const T& data) {
		updateDataAll((void*)&data, sizeof(T));
	}

	template<typename T>
	T getData() {
		T data;
		get_data((void*)&data, sizeof(T));
		return data;
	}

	void updateData(void* pData, uint nData);

	void updateDataAll(void* pData, uint nData);

	bool isAllocated();
    void build();
	void cleanup() override;
private:
    void onResize();
	void get_data(void* d, uint n);
	friend class CommandBuffer_API_data;
	uptr<UniformBuffer_API_Data> api_data;
    wptr<Shader> shader;
    struct Binding {
        uint binding;
        uint size;
    } binding;

    EventRegistration resizeReg;
};

NSP_GL_END
