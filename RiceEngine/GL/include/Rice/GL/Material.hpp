#include "stdafx.hpp"

NSP_GL_BEGIN

class Material;

NSP_GL_END

#pragma once

#include "UniformBuffer.hpp"
#include "GraphicsManager.hpp"
#include "Shader.hpp"
//TODO #include "Texture2D.hpp"

NSP_GL_BEGIN

	enum class RenderType { Solid = 0, Transparent = 1 };

	class Material {
	public:
		RenderType renderType = RenderType::Solid;

		Material(ptr<GraphicsManager> manager, ptr<Shader> shader);
		~Material() {}

		void updateBuffer();

		/*void AddTexture(pTexture2D tex);
		pTexture2D GetTexture(size_t idx);
		const TextureArray GetTextures() const;*/

		ptr<Shader> getShader() { return shader; }
        
        //can be null
		ptr<UniformBuffer> getUniformBuffer() { return uniformBuffer; }
	private:
		ptr<Shader> shader;
		ptr<UniformBuffer> uniformBuffer;
		ptr<GraphicsManager> graphics_manager;
		//TODO vec<pTexture2D> textureArr{};
	};

NSP_GL_END
