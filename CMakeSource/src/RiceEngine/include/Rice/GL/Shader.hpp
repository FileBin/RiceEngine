#include "../stdafx.hpp"
NSP_GL_BEGIN
class Shader;
typedef SmartPtr<Shader> pShader;
NSP_GL_END

#pragma once

#include "GraphicsManager.hpp"
#include "GraphicsComponentBase.hpp"

NSP_GL_BEGIN
class Shader : public GraphicsComponentBase {
public:
	enum Type {
		Vertex, Geometry, Fragment,
	};
	Shader(pGraphicsManager g_mgr);
	~Shader() override;

	void cleanup() override;

	void loadShader(String path, Type shaderType);

	void buildPipeline(Vector2i windowSize);

	void setActive();
private:
	vk::ShaderModule vertexShader, fragmentShader;
	vk::Pipeline pipeline;
	vk::PipelineLayout layout;

	bool init = false;
	bool uses_depth_buffer = false;
};
NSP_GL_END

