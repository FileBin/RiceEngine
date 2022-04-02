#include "../stdafx.hpp"

NSP_GL_BEGIN

class Material;
typedef SmartPtr<Material> pMaterial;

NSP_GL_END

#pragma once
#include "GraphicsManager.hpp"
#include "Shader.hpp"
#include "Texture2D.hpp"

NSP_GL_BEGIN

	enum class RenderType { Solid = 0, Transparent = 1 };

	class Material {
	public:
		RenderType renderType = RenderType::Solid;

		Material(pGraphicsManager manager, pShader sh);
		~Material() {}

		void UpdateBuffer();

		//TODO make set VAR
		/*template<typename T>
		void SetVar(String name, T val) {
			mapping.SetData(name, &val, sizeof(val));
		}*/

		void AddTexture(pTexture2D tex);
		pTexture2D GetTexture(size_t idx);
		const TextureArray GetTextures() const;

		pShader GetShader() { return shader; }
		//Buffer* GetBuffer() { return constantBuffer; }
	private:
		pShader shader;
		//Buffer* constantBuffer = nullptr;
		pGraphicsManager graphics_manager;
		vec<pTexture2D> textureArr{};
	};

NSP_GL_END
