#pragma once
#include "Shader.h"

namespace Game {

	class Device;

	enum class RenderType { Solid = 0, Transparent = 1 };

	class Material {
	public:
		RenderType renderType = RenderType::Solid;

		Material(data_t data, Device& d, Shader& sh);

		Shader& GetShader() { return *shader; }
		Buffer* GetBuffer() { return constantBuffer; }
	private:
		Shader* shader = nullptr;
		Buffer* constantBuffer = nullptr;
	};
}