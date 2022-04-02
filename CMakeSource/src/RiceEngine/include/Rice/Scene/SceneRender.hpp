#include "../stdafx.hpp"

NSP_SCENING_BEGIN
class SceneRender;
typedef SmartPtr<SceneRender> pSceneRender;
NSP_SCENING_END

#pragma once

#include "../Math/Hasher.hpp"
#include "../Math.hpp"
#include "IPostProcess.hpp"
#include "../Components/UI/IDrawable.hpp"
#include "../Components/ModelRender.hpp"
#include "../Components/Transform.hpp"
#include "../Components/UI/Canvas.hpp"
#include "../GL/RenderingMesh.hpp"
#include "../Engine/RenderBase.hpp"
#include "../Util/RegisterCollection.hpp"
#include "../GL/Texture2D.hpp"
#include "IRenderable.hpp"
#include "../GL/Material.hpp"
#include "Camera.hpp"

NSP_SCENING_BEGIN

class SceneRender : public ::Rice::RenderBase {
private:
	using pIPPScript = Graphics::pIPostProcess;
	using pIDrawable = UI::pIDrawable;
	using pTex = Graphics::pTexture2D;
	using texArr = Graphics::TextureArray;
	using pMat = Graphics::pMaterial;
	using pShader = Graphics::pShader;
	using pGrMgr = Graphics::pGraphicsManager;
#pragma region Classes
	class LightManager;

	class LightManager {
		struct LightBuffer {
			Vector4f ambient = {};
			Vector4f diffuse = {};
			Vector4f lightDir = {};
		}buf;
		Graphics::pTexture2D shadowAtlas;
		pSceneRender sceneRender;

		size_t numCascades = 0;
		Vector3 lightDirection = {};
		dbl shadowDistance = 300;
		size_t shadowMapRes = 0;
		_STD vector<dbl> shadowMapSizes = {};
		Matrix4x4 LVP = Matrix4x4::identity;
	public:
		void updateBuffer();
		void preInit(pSceneRender ren, vec<dbl> mapSizes, dbl shadowDistanse = 600, size_t shadowMapRes = 1024);
		void renderShadowMap(Vector3 playerPos, const vec<pIRenderable>& meshes);

		Matrix4x4 getMatrixLVP() {return LVP;}

		Graphics::pTexture2D getShadowMap() {return shadowAtlas.get();}
		//Buffer* GetBuffer() { return lightBuffer.Get(); }

	}lightManager;

	friend class LightManager;
	UI::Canvas canvas;
#pragma endregion
public:
	SceneRender(Scene* scene);
	bool init();
	void beginFrame();
	bool draw();
	void close();
	void resize();

	//Buffer* GetConstBuffer() { return constantBuffer.Get(); }

	void setupSkybox(Graphics::Material skyboxMat);

	void registerModel(::Rice::pIRenderable ren);
	void unregisterModel(::Rice::pIRenderable ren);

	void addCamera(pCamera cam);
	pCamera getCamera(size_t idx);
	pCamera getActiveCamera() {return getCamera(activeCameraIdx);}

	Graphics::pShader createShader(String name);
	Graphics::pShader getShader(String name);

	Graphics::pMaterial createMaterial(String name, Graphics::pShader sh);
	Graphics::pMaterial getMaterial(String name);

//	Graphics::pTexture2D GetDepthBufferTex() {return *device->GetDepthBufferTex();}
//	Graphics::pTexture2D GetRenderTargetTex() {return *device->GetRenderTargetTex();}

	void addDrawable(pIDrawable txt);
	void removeDrawable(pIDrawable txt);

	void postProcess(pMat mat);
	void defaultPostProcess();

	void addPostProcessScript(pIPPScript ppscript) {ppscripts.insert(ppscript);}
	void removePostProcessScript(pIPPScript ppscript) {ppscripts.erase(ppscript);}

private:
	SmartPtr<Scene> scene;

	size_t activeCameraIdx = 0;
	_STD mutex m_mutex, m_2dMutex, m_removeMutex;
	vec<pCamera> cameras;
	umap<String, pMat> materials;
	umap<String, pShader> shaders;
	RegisterCollection<pIRenderable> renderingMeshes;
	uset<pIPPScript> ppscripts;
	vec<SmartPtr<UI::IDrawable>> drawables;
	//Microsoft::WRL::ComPtr<Buffer> constantBuffer;

	//default
	SmartPtr<RenderingMesh> skyBox, postProcessingQuad;
	pMat skyboxMaterial, defaultPostProcessMaterial;

	Mesh* CreateSkyBoxMesh();
};
NSP_SCENING_END
