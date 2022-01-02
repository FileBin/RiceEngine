#include "pch.h"
#include <GameEngine\Material.h>
#include <GameEngine\Device.h>

namespace Game {
	Material::Material(Device& dev, Shader& sh, std::vector<std::pair<String, size_t>> m) {
		mapping = { m };
		shader = &sh;
		d = &dev;
		auto data = mapping.GetData();
		if (data.size() > 0) {
			constantBuffer = d->CreateBuffer(data, data.size(), D3D11_BIND_CONSTANT_BUFFER);
		}

	}
	void Material::UpdateBuffer() {
		if (constantBuffer)
			d->LoadBufferSubresource(constantBuffer, mapping.GetData()[0]);
	}
	void Material::AddTexture(Texture2D* tex) {
		textureArr.push_back(tex);
	}
	Texture2D& Material::GetTexture(size_t idx) {
		return *textureArr[idx];
	}
	const std::vector<Texture2D*> Material::GetTextures() const {
		return textureArr;
	}
	Material::Mapping::Mapping(std::vector<std::pair<String, size_t>> mapping) {
		size_t bufferSize = 0;
		for (auto arg : mapping) {
			map.insert({ arg.first, bufferSize });
			bufferSize += arg.second;
		}
		data.resize(bufferSize);
		ZeroMemory(data.data(), bufferSize);
	}
	void Material::Mapping::SetData(String name, void* pData, size_t nData) {
		void* dst = data.data() + map.at(name);
		memcpy(dst, pData, nData);
	}
	const data_t Material::Mapping::GetData() {
		return data;
	}
}