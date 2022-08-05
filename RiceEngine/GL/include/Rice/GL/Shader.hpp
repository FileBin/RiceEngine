#include "stdafx.hpp"
NSP_GL_BEGIN

class Shader;
struct Shader_API_Data;

NSP_GL_END

#pragma once

#include "GraphicsManager.hpp"
#include "GraphicsComponentBase.hpp"
#include "VertexLayout.hpp"
#include "Rice/Math/Math.hpp"

NSP_GL_BEGIN
class Shader : public GraphicsComponentBase {
public:
	enum Type {
		Vertex, Geometry, Fragment,
	};
	Shader(ptr<GraphicsManager> g_mgr);
	~Shader() override;

	void cleanup() override;

	void loadShader(String path, Type shaderType);

	void addUniformBuffer(uint binding, Type shaderStage);

	void setVertexStride(uint stride) {
		vertexStride = stride;
	}

	void setVertexLayout(VertexLayout layout) {
		vertexLayout = layout;
	}

	template<typename VertT>
	void setVertexStrideAndLayout() {
		vertexStride = sizeof(VertT);
		vertexLayout = VertT::vertexLayout;
	}

	void build();
private:
	friend struct CommandBuffer_API_data;
	friend class UniformBuffer;
    friend class UniformBuffer_API_Data;
	uptr<Shader_API_Data> api_data;

	Util::EventRegistration resizeReg;

	uint vertexStride = 0;
	VertexLayout vertexLayout;

	bool init = false;
	bool uses_depth_buffer = false;

	void onResize(Vector2i windowSize);
};
NSP_GL_END

