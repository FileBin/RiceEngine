#pragma once
#include "Shader.h"
#include <unordered_map>
#include "DX\Texture2D.h"

namespace Game {

	template<typename T = Vector4f>
	std::pair<String, size_t> Var(const String s) {
		return { s, sizeof(T) };
	}

	class Device;

	enum class RenderType { Solid = 0, Transparent = 1 };

	class Material {
	public:
		RenderType renderType = RenderType::Solid;

		Material(Device& dev, Shader& sh, std::vector<std::pair<String, size_t>> mapping = {});
		~Material() {}

		void UpdateBuffer();

		template<typename T>
		void SetVar(String name, T val) {
			mapping.SetData(name, &val, sizeof(val));
		}

		void AddTexture(Texture2D* tex);
		Texture2D& GetTexture(size_t idx);
		const std::vector<Texture2D*> GetTextures() const;

		Shader& GetShader() { return *shader; }
		Buffer* GetBuffer() { return constantBuffer; }
	private:
		Shader* shader = nullptr;
		Buffer* constantBuffer = nullptr;
		Device* d;
		std::vector<Texture2D*> textureArr{};

		class Mapping {
		private:
			std::unordered_map<String, size_t> map{};
			data_t data{};
		public:
			Mapping(std::vector<std::pair<String, size_t>> mapping = {});
			void SetData(String name, void* pData, size_t nData);
			const data_t GetData();
		} mapping{};
	};
}