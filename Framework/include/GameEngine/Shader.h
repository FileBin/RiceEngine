#pragma once
#include "stdafx.h"
#include "Util.h"

namespace Game {
	class Device;

	class Shader {
	public:
		Shader(Device* device);
		~Shader();



		void LoadVertexShader(std::vector<byte> shaderData,
			std::vector<D3D11_INPUT_ELEMENT_DESC> layout = Util::Vertex::GetLayout());
		void LoadPixelShader(std::vector<byte> shaderData);

		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;
	private:
		Device* device;
	};
}