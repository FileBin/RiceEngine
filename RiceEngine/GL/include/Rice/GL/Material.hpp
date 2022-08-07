#include "stdafx.hpp"

NSP_GL_BEGIN

class Material;

NSP_GL_END

#pragma once

#include "GraphicsManager.hpp"
#include "Shader.hpp"
#include "UniformBuffer.hpp"
// TODO #include "Texture2D.hpp"

NSP_GL_BEGIN

enum class RenderType { Solid = 0, Transparent = 1 };

class Material {
  public:
    RenderType renderType = RenderType::Solid;

    Material(ptr<GraphicsManager> manager, ptr<Shader> shader);

    template <typename T> void initUniformBuffer(const T &init_data) {
        initUniformBuffer(sizeof(T));
        uniform_buffer->updateData<T>(init_data);
    }

    template <typename T> void initUniformBuffer() {
        initUniformBuffer(sizeof(T));
    }

    void initUniformBuffer(size_t size) {
        uniform_buffer = new_ptr<UniformBuffer>(graphics_manager, size);
    }

    ~Material() {}

    void updateBuffer();

    /*void AddTexture(pTexture2D tex);
    pTexture2D GetTexture(size_t idx);
    const TextureArray GetTextures() const;*/

    ptr<Shader> getShader() { return shader; }

    // can be nullptr if no buffer is set
    ptr<UniformBuffer> getUniformBuffer() { return uniform_buffer; }

  private:
    ptr<Shader> shader;
    ptr<UniformBuffer> uniform_buffer;
    ptr<GraphicsManager> graphics_manager;
    // TODO vec<pTexture2D> textureArr{};
};

NSP_GL_END
