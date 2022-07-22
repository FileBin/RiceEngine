#include "../stdafx.hpp"
#include "BetterCpp/Objects/PtrTypes.hpp"
#include "Rice/GL/UniformBuffer.hpp"

NSP_GL_BEGIN

class Material;
typedef RefPtr<Material> pMaterial;

NSP_GL_END

#pragma once
#include "GraphicsManager.hpp"
#include "Shader.hpp"
//TODO #include "Texture2D.hpp"

NSP_GL_BEGIN

	enum class RenderType { Solid = 0, Transparent = 1 };

	class Material {
	public:
		RenderType renderType = RenderType::Solid;

		Material(pGraphicsManager manager, pShader sh);
		~Material() {}

		void updateBuffer();

		/*void AddTexture(pTexture2D tex);
		pTexture2D GetTexture(size_t idx);
		const TextureArray GetTextures() const;*/

		pShader getShader() { return shader; }
		pUniformBuffer getUniformBuffer() { return uniformBuffer; }
	private:
		pShader shader;
		pUniformBuffer uniformBuffer;
		pGraphicsManager graphics_manager;
		//TODO vec<pTexture2D> textureArr{};
	};

NSP_GL_END
