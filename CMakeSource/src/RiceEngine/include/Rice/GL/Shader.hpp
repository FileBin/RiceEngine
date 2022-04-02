#include "../stdafx.hpp"
NSP_GL_BEGIN
class Shader;
typedef SmartPtr<Shader> pShader;
NSP_GL_END

#pragma once

#include "GraphicsManager.hpp"

NSP_GL_BEGIN
class Shader {
public:
	Shader();
	~Shader();

	void loadVertexShader(String filename); //, VertexLayout layout = Vertex::GetLayout());
	//void loadPixelShader(String filename);

	//TODO make shaders
	//ID3D11VertexShader* vertexShader;
	//ID3D11PixelShader* pixelShader;
	//ID3D11InputLayout* layout;

	bool uses_depth_buffer = false;
private:
	pGraphicsManager graphics_manager;
};
NSP_GL_END

