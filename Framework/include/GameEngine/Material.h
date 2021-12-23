#pragma once
#include "Shader.h"

namespace Game {

	class Device;

	class Material {
	public:
		Material(data_t data, Device& d, Shader& sh);

		Shader& GetShader() { return *shader; }
		Buffer* GetBuffer() { return constantBuffer; }
	private:
		Shader* shader = nullptr;
		Buffer* constantBuffer = nullptr;
	};
}