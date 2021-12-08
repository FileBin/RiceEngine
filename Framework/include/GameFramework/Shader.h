#pragma once
#include "stdafx.h"

namespace GameEngine {
	class Device;

	class Shader {
	public:
		Shader(Device* device);
		~Shader();



		void LoadVertexShader(std::vector<byte> shaderData,
			std::vector<D3D11_INPUT_ELEMENT_DESC> layout = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } });
		void LoadPixelShader(std::vector<byte> shaderData);

		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;
	private:
		Device* device;
	};
}