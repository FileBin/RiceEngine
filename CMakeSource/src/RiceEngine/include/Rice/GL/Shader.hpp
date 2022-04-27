#include "../stdafx.hpp"
NSP_GL_BEGIN
class Shader;
typedef SmartPtr<Shader> pShader;

struct Shader_API_Data;
typedef SmartPtr<Shader_API_Data> pShader_API_Data;
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
	pShader_API_Data api_data;

	GraphicsManager::ResizeEvent::UUID on_resize_uuid;

	bool init = false;
	bool uses_depth_buffer = false;

	void onResize(Vector2i windowSize);
	void cleanupPipeline();
	void cleanupShaders();
};
NSP_GL_END

