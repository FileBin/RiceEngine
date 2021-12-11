#include <GameEngine/Shader.h>
#include <GameEngine/macros.h>
#include <GameEngine/Device.h>

namespace GameEngine {
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

	void Shader::LoadVertexShader(std::vector<byte> shaderData, std::vector<D3D11_INPUT_ELEMENT_DESC> layout) {
		vertexShader = device->CreateVertexShader(shaderData);
		this->layout = device->CreateInputLayout(layout, shaderData);
	}

	void Shader::LoadPixelShader(std::vector<byte> shaderData) {
		pixelShader = device->CreatePixelShader(shaderData);
	}
}
