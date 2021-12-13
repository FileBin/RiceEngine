#include <GameEngine\Material.h>
#include <GameEngine\Device.h>

namespace Game {
	Material::Material(data_t data, Device& d, Shader& sh) {
		if (data.size() > 0) {
			constantBuffer = d.CreateBuffer(data, data.size(), D3D11_BIND_CONSTANT_BUFFER);
		}
		shader = &sh;
	}
}