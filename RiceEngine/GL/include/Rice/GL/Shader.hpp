#include "stdafx.hpp"
NSP_GL_BEGIN

class Shader;
struct Shader_API_Data;

NSP_GL_END

#pragma once

#include "GraphicsManager.hpp"
#include "GraphicsComponentBase.hpp"
#include "VertexLayout.hpp"
#include "Rice/Math/Math.hpp"

NSP_GL_BEGIN
class Shader : public GraphicsComponentBase {
public:
	enum Type {
		Vertex, Geometry, Fragment,
	};
	Shader(ptr<GraphicsManager> g_mgr);
	~Shader() override;

	void cleanup() override;

	void loadShader(String path, Type shaderType);

	void addUniformBuffer(uint binding, Type shaderStage);

	/*
	 * Same, for one block that both stages read. A block declared twice through
	 * the single-stage overload would put two layout bindings with the same
	 * number into the descriptor set layout, which is invalid; declaring the
	 * binding once with both stage flags is the fix. Used by the game's water
	 * shader: `time` animates the surface in the vertex stage and the normals
	 * in the fragment stage.
	 */
	void addUniformBuffer(uint binding, Type firstStage, Type secondStage);


	/*
	 * Declares the scene lighting resources for this shader, i.e. what the
	 * scene shaders built with -DRICE_LIGHTING use (Tests/shaders/scene/
	 * light.glsl + shadow.glsl):
	 *
	 *   light_binding  (default 2) uniform LightData, vertex AND fragment
	 *   shadow_binding (default 3) sampler2D shadow_map, fragment
	 *
	 * SceneRender binds both on every draw of a shader that declared them,
	 * for as long as the scene has an enabled Components::DirectionalLight.
	 * A shader without this call keeps a layout of just the bindings it
	 * declared and is drawn exactly as before. Call before build().
	 */
	void addLighting(uint light_binding = 2, uint shadow_binding = 3);

	/**
	 * True when addLighting() was called: SceneRender then has to bind the
	 * light block and the shadow map before drawing with this shader.
	 */
	bool usesLighting() const { return lighting; }
	uint getLightBinding() const { return light_binding; }
	uint getShadowBinding() const { return shadow_binding; }

	void setVertexStride(uint stride) {
		vertexStride = stride;
	}

	void setVertexLayout(VertexLayout layout) {
		vertexLayout = layout;
	}

	template<typename VertT>
	void setVertexStrideAndLayout() {
		vertexStride = sizeof(VertT);
		vertexLayout = VertT::vertexLayout;
	}

	void build();

	/**
	 * Opt-in alpha blending: call before build() (and before buildShadowPipeline(),
	 * which ignores it - the casters pass has no colour attachment).
	 *
	 * build() then creates the pipeline with
	 *
	 *   srcAlpha / one-minus-srcAlpha blending on colour and alpha,
	 *   depth test ON with LESS (unchanged), depth write OFF.
	 *
	 * Drawing without depth writes is what makes blended geometry not occlude
	 * what is behind it; the engine also keeps such meshes out of the shadow
	 * casters pass and draws them after the opaque ones (see Material::
	 * isTransparent and SceneRender). Blend is OFF by default, so every existing
	 * shader keeps its opaque pipeline.
	 */
	void setBlendEnabled(bool enabled = true) { blend_enabled = enabled; }
	bool isBlendEnabled() const { return blend_enabled; }

	/**
	 * Builds a depth-only casters pipeline instead of the scene pipeline: the
	 * shadow map's render pass (single D32_SFLOAT attachment, no colour target),
	 * depth test + write with LESS, both faces drawn, and no fragment stage —
	 * the shadow pass only needs the depth values (load the vertex shader only
	 * and pass the per-mesh ModelData at binding 0 plus the scene LightData at
	 * binding 2 with addUniformBuffer()). `map_size` is the ShadowMap size and
	 * becomes the viewport, so it must match the map that is rendered into.
	 *
	 * Such a pipeline is not rebuilt on window resize (it does not depend on the
	 * swapchain); use it only for ShadowMap rendering, never for the scene.
	 */
	void buildShadowPipeline(uint map_size);

private:
	friend struct CommandBuffer_API_data;
	friend class UniformBuffer;
    friend class UniformBuffer_API_Data;
	uptr<Shader_API_Data> api_data;

	Util::EventRegistration resizeReg;

	uint vertexStride = 0;
	VertexLayout vertexLayout;

	bool init = false;
	bool uses_depth_buffer = false;
	// buildShadowPipeline(): depth-only pass pipeline, skip resize rebuilds.
	bool shadow_pipeline = false;
	// setBlendEnabled(): alpha blended scene pipeline with depth writes off.
	bool blend_enabled = false;
	// addLighting(): the descriptor bindings SceneRender has to fill in.
	bool lighting = false;
	uint light_binding = 2;
	uint shadow_binding = 3;

	void onResize(Vector2i windowSize);
};
NSP_GL_END

