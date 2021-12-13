#pragma once
#include "stdafx.h"
#include "Util.h"

namespace Game {
	class Device;

	class Shader {
	public:
		Shader(Device* device);
		~Shader();

		void LoadVertexShader(data_t shaderData,
			VertexLayout layout = Util::Vertex::GetLayout());
		void LoadPixelShader(data_t shaderData);

		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;
	private:
		Device* device;
	};
}