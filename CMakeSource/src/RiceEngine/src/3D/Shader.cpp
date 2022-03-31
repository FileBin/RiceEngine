#include "pch.h"
#include <GameEngine/Shader.h>
#include <GameEngine/macros.h>
#include <GameEngine/Device.h>

namespace Game {
	Shader::Shader(Device* device) : device(device) {
		pixelShader = nullptr;
		vertexShader = nullptr;
		layout = nullptr;
	}

	Shader::~Shader() {
		device = nullptr;
		_RELEASE(pixelShader);
		_RELEASE(vertexShader);
		_RELEASE(layout);
	}

	void Shader::LoadVertexShader(data_t shaderData, VertexLayout layout) {
		vertexShader = device->CreateVertexShader(shaderData);
		this->layout = device->CreateInputLayout(layout, shaderData);
	}

	void Shader::LoadPixelShader(data_t shaderData) {
		pixelShader = device->CreatePixelShader(shaderData);
	}
}
