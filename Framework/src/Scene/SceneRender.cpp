#include "pch.h"
#include <GameEngine\Scene\SceneRender.h>
#include <GameEngine\Material.h>
#include <GameEngine\Model.h>
#include <GameEngine\Camera.h>
#include <GameEngine\Util\exceptions.h>
#include <queue>
#include <GameEngine\Components\UI\IDrawable.h>
#include <GameEngine\Scene\IRenderable.h>
#include <GameEngine\Log.h>

namespace Game {

	bool SceneRender::Init() {
		canvas.referenceResoluton = { 800,600 };
		lightManager.PreInit(this, { 150 });

		constantBuffer = device->CreateBuffer<ConstantBufferData>({}, D3D11_BIND_CONSTANT_BUFFER);
		auto mesh = new RenderingMesh();
		mesh->pIndexBuffer = device->CreateBuffer<UINT>({ 2,1,0,0,3,2 }, D3D11_BIND_INDEX_BUFFER);
		mesh->pVertexBuffer = device->CreateBuffer<Vertex>({
				{ {-1.f, -1.f, 0 } },
				{ {1.f, -1.f, 0 } },
				{ {1.f, 1.f, 0 } },
				{ {-1.f, 1.f, 0 } },
			}, D3D11_BIND_VERTEX_BUFFER);

		mesh->pConstBuffer = constantBuffer;
		mesh->device = device;
		postProcessingQuad = mesh;

		return true;
	}

	void Game::SceneRender::SetupSkybox(SmartPtr<Material> skyboxMat) {
		auto rmesh = new RenderingMesh();

		auto mesh = CreateSkyBoxMesh();

		rmesh->pVertexBuffer = device->CreateBuffer(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER);
		rmesh->pIndexBuffer = device->CreateBuffer(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER);
		rmesh->orig = mesh;
		rmesh->mat = skyboxMaterial = skyboxMat;
		rmesh->transform = new Transform();
		rmesh->pConstBuffer = constantBuffer;
		rmesh->device = device;
		skyBox = rmesh;
	}

	void SceneRender::BeginFrame() {
		device->SetRenderTargetsDefault();
		device->ClearFrame({ 0.1f, 0.15f, 0.6f, 1.f });
		lightManager.UpdateBuffer();
		if (!skyBox.IsNull()) {
			auto cam = cameras[activeCameraIdx];
			skyBox->Render(Matrix4x4::Rotation(cam->rotation.Opposite()), cam->GetProjectionMatrix(), Matrix4x4::identity);
			device->ClearZBuffer();
		}
	}

	bool SceneRender::Draw() {
		auto cam = *cameras[activeCameraIdx];
		device->SetPrimitiveTopology();
		device->SetBlendState(false);
		device->UseDepthBuffer(true);
		Matrix4x4 mV = cam.GetTransformationMatrix();
		Matrix4x4 mP = cam.GetProjectionMatrix();
		using namespace std::chrono;

		device->SetRSState(false);

		m_mutex.lock();

		const auto& meshes = renderingMeshes.GetCollectionWithGaps();

		device->SetActiveVSConstantBuffer(constantBuffer.Get());
		lightManager.RenderShadowMap(cam.position, meshes);

		auto LVP = lightManager.GetMatrixLVP();

		device->SetVPDefault();
		device->SetRenderTargetsDefault();
		device->SetRSState();

		device->SetPSTexture2D(lightManager.GetShadowMap(), 13);
		device->SetActiveVSConstantBuffer(lightManager.GetBuffer(), 13);
		device->SetActivePSConstantBuffer(lightManager.GetBuffer(), 13);
		auto n = meshes.size();
		auto pArr = meshes.data();

		for (size_t i = 0; i < n;i++) {
			auto elem = *(pArr + i);
			if (elem.IsNull()) continue;
			elem->Render(mV, mP, LVP);
		}
		device->SetBlendState(true);
		device->CopyBuffers();
		device->SetRenderTargetsDefault();

		for (size_t i = 0; i < n; i++) {
			auto elem = *(pArr + i);
			if (elem.IsNull()) continue;
			elem->RenderTransparent(mV, mP, LVP);
		}
		m_mutex.unlock();

		for (auto pps : ppscripts) {
			pps->PostProcess();
		}

		device->Begin2D();
		m_2dMutex.lock();
		for (auto& d : drawables) {
			d->Draw(device);
		}
		m_2dMutex.unlock();
		device->End2D();

		return true;
	}

	void SceneRender::PostProcess(Material* mat) {
		device->CopyBuffers();
		device->SetVPDefault();
		device->SetRenderTargetsDefault();

		device->SetBlendState(false);

		device->SetActiveVertexBuffer<Vertex>(postProcessingQuad->pVertexBuffer.Get());
		device->SetActiveIndexBuffer(postProcessingQuad->pIndexBuffer.Get());
		device->SetActiveVSConstantBuffer(mat->GetBuffer(), 1);
		device->SetActivePSConstantBuffer(mat->GetBuffer());
		device->SetPSTextures(mat->GetTextures());

		device->SetActiveShader(mat->GetShader());

		device->UseDepthBuffer(false);
		device->Draw();
	}

	void SceneRender::Close() {
		for (auto& m : shaders) { m.second.Release(); }
		for (auto& m : materials) { m.second.Release(); }
		_RELEASE(constantBuffer);
	}

	void SceneRender::Resize() {
		auto screenSize = Util::GetWindowScreenSize(device->GetHWND());
		canvas.currentResolution = screenSize;
		auto n = cameras.size();
		auto ar = GetAspectRatio();
		for (size_t i = 0; i < n; i++) {
			cameras[i]->SetAspectRatio(ar);
		}
	}

	void SceneRender::RegisterModel(SmartPtr<IRenderable> ren) {
		std::lock_guard lock(m_mutex);
		ren->Register(renderingMeshes.Register(ren));
	}

	void SceneRender::UnregisterModel(SmartPtr<IRenderable> rm) {
		std::lock_guard lock(m_mutex);
		renderingMeshes.Unregister(rm->Unregister());
	}

	void SceneRender::AddCamera(SmartPtr<Camera> cam) { cameras.push_back(cam); }

	SmartPtr<Camera> SceneRender::GetCamera(size_t idx) { return { cameras[idx] }; }

	SmartPtr<Shader> SceneRender::CreateShader(String name) {
		auto sh = new Shader(device);
		shaders.insert({name, sh});
		return sh;
	}

	SmartPtr<Shader> SceneRender::GetShader(String name) {
		try {
			return shaders.at(name);
		} catch (std::out_of_range) {
			THROW_INVALID_ARG_EXCEPTION("name");
		}
	}

	SmartPtr<Material> SceneRender::CreateMaterial(String name, SmartPtr<Shader> sh, std::vector<std::pair<String, size_t>> mapping) {
		SmartPtr<Material> mat = new Material(*device, *sh, mapping);
		materials.insert(materials.end(), { name, mat });
		return mat;
	}
	SmartPtr<Material> SceneRender::GetMaterial(String name) {
		try {
			return materials.at(name);
		} catch (std::out_of_range) {
			THROW_INVALID_ARG_EXCEPTION("name");
		}
	}
	void SceneRender::AddDrawable(UI::IDrawable* txt) {
		std::lock_guard lock(m_2dMutex);
		txt->SetCanvas(&canvas);
		drawables.push_back(txt);
	}
	void SceneRender::RemoveDrawable(UI::IDrawable* txt) {
		for (auto it = drawables.begin(); it != drawables.end(); it++) {
			if (it->Get() == txt) {
				std::lock_guard lock(m_2dMutex);
				drawables.erase(it);
				break;
			}
		}
	}
	Mesh* SceneRender::CreateSkyBoxMesh() {
		Mesh* mesh = new Mesh();

		constexpr dbl onesixth = 1. / 6.;

		constexpr dbl eps = 0.00698;

		const Vector2f texcoords[4] = {
			{ (float)(onesixth * (1. - eps)), (float)(1. - eps) },
			{ (float)(eps*onesixth), (float)(1. - eps) },
			{ (float)(eps*onesixth), (float)eps },
			{ (float)(onesixth*(1. - eps)), (float)eps },
		};

		auto quad = Mesh::quad;
		quad.Translate({ 0, 0, -.5f });//-Z

		for (auto i = 0; i < 4; i++) {
			quad.vertexBuffer[i].texcoord = texcoords[i];
		}

		mesh->Combine(quad);

		for (auto i = 1; i < 4; i++) {
			quad.Rotate(Quaternion::FromAxisAngle(Vector3::up, 90)); //+Z, +-X

			for (auto j = 0; j < 4; j++) {
				quad.vertexBuffer[j].texcoord = texcoords[j];
				quad.vertexBuffer[j].texcoord.x += onesixth * i;
			}

			mesh->Combine(quad);
		}

		quad = Mesh::quad;
		quad.Translate({ 0, 0, -.5f });
		quad.Rotate(Quaternion::FromAxisAngle(Vector3::right, -90)); //-Y

		for (auto i = 0; i < 4; i++) {
			quad.vertexBuffer[i].texcoord = texcoords[i];
			quad.vertexBuffer[i].texcoord.x += onesixth * 4;
		}

		mesh->Combine(quad);

		quad = Mesh::quad;
		quad.Translate({ 0, 0, -.5f });
		quad.Rotate(Quaternion::FromAxisAngle(Vector3::right, 90)); //+Y

		for (auto i = 0; i < 4; i++) {
			quad.vertexBuffer[i].texcoord = texcoords[i];
			quad.vertexBuffer[i].texcoord.x += onesixth * 5;
		}

		mesh->Combine(quad);
		mesh->ReclaculateBounds();
		mesh->RecalculateNormals();

		return mesh;
	}
}