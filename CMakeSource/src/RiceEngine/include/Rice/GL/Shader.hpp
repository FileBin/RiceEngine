#include "../stdafx.hpp"
NSP_GL_BEGIN
class Shader;
typedef RefPtr<Shader> pShader;

struct Shader_API_Data;
typedef RefPtr<Shader_API_Data> pShader_API_Data;
NSP_GL_END

#pragma once

#include "GraphicsManager.hpp"
#include "GraphicsComponentBase.hpp"
#include "../Math.hpp"

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
private:
	friend class CommandBuffer_API_data;
	AutoPtr<Shader_API_Data> api_data;

	EventRegistration resizeReg;

	bool init = false;
	bool uses_depth_buffer = false;

	void onResize(Vector2i windowSize);
	void cleanupPipeline();
	void cleanupShaders();
};
NSP_GL_END

