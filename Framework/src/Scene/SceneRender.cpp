#include "pch.h"
#include <GameEngine\SceneRender.h>
#include <GameEngine\Material.h>
#include <GameEngine\Model.h>
#include <GameEngine\Camera.h>
#include <GameEngine\Util\exceptions.h>
#include <queue>
#include <GameEngine\Components\UI\IDrawable.h>
#include <GameEngine\Log.h>

namespace Game {

	bool SceneRender::Init() {
		lightManager.PreInit(this, { 150 });

		constantBuffer = device->CreateBuffer<ConstantBufferData>({}, D3D11_BIND_CONSTANT_BUFFER);

		postProcessingQuad = new RenderingMesh();
		postProcessingQuad->pIndexBuffer = device->CreateBuffer<UINT>({ 2,1,0,0,3,2 }, D3D11_BIND_INDEX_BUFFER);
		postProcessingQuad->pVertexBuffer = device->CreateBuffer<Vertex>({
				{ {-1.f, -1.f, 0 } },
				{ {1.f, -1.f, 0 } },
				{ {1.f, 1.f, 0 } },
				{ {-1.f, 1.f, 0 } },
			}, D3D11_BIND_VERTEX_BUFFER);

		return true;
	}

	void Game::SceneRender::SetupSkybox(SmartPtr<Material> skyboxMat) {
		skyBox = new RenderingMesh();

		auto mesh = CreateSkyBoxMesh();

		skyBox->pVertexBuffer = device->CreateBuffer(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER);
		skyBox->pIndexBuffer = device->CreateBuffer(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER);
		skyBox->orig = mesh;
		skyboxMaterial = skyboxMat;
		skyBox->transform = new Transform();
	}

	void SceneRender::BeginFrame() {
		device->SetRenderTargetsDefault();
		device->ClearFrame({ 0.1f, 0.15f, 0.6f, 1.f });
		lightManager.UpdateBuffer();
		if (!skyBox.IsNull()) {
			auto cam = cameras[activeCameraIdx];
			setActiveMaterial(skyboxMaterial);
			skyBox->Draw(this, Matrix4x4::Rotation(cam->rotation.Opposite()), cam->GetProjectionMatrix(), Matrix4x4::identity, false);
			device->ClearZBuffer();
		}
	}

	void SceneRender::RenderingMesh::Draw(SceneRender* ren, Matrix4x4f View, Matrix4x4f Projection, Matrix4x4 LVP, bool check) {
		auto device = ren->device;
		auto constantBuffer = ren->constantBuffer.Get();
		ConstantBufferData cb = {};
		auto World = transform->GetTransformationMatrix();
		cb.World = World;
		cb.WorldView = cb.World * View;
		cb.Projection = Projection;
		cb.LightWVP = World * LVP;
		if (check)
			if (orig.IsNull() || !orig->CheckVisiblity(cb)) return;

		device->LoadBufferSubresource(constantBuffer, cb);
		device->SetActiveVSConstantBuffer(constantBuffer);

		device->SetActiveVertexBuffer<Vertex>(pVertexBuffer.Get());
		device->SetActiveIndexBuffer(pIndexBuffer.Get());

		device->Draw();
	}

	bool SceneRender::Draw() {
		auto cam = *cameras[activeCameraIdx];
		device->SetPrimitiveTopology();
		device->SetBlendState(false);
		device->UseDepthBuffer(true);
		Matrix4x4f mV = cam.GetTransformationMatrix();
		Matrix4x4f mP = cam.GetProjectionMatrix();
		using namespace std::chrono;

		m_mutex.lock();
		updateMeshes();
		m_mutex.unlock();

		device->SetRSState(false);

		lightManager.RenderShadowMap(cam.position, renderingMeshes);

		auto LVP = lightManager.GetMatrixLVP();

		device->SetVPDefault();
		device->SetRenderTargetsDefault();
		device->SetRSState();

		device->SetPSTexture2D(lightManager.GetShadowMap(), 13);
		device->SetActiveVSConstantBuffer(lightManager.GetBuffer(), 13);
		device->SetActivePSConstantBuffer(lightManager.GetBuffer(), 13);

		for (auto& c : renderingMeshes) {
			setActiveMaterial(c.first);
			for (auto& m : c.second) {
				m.second->Draw(this, mV, mP, LVP);
			}
		}
		device->SetBlendState(true);
		device->CopyBuffers();
		device->SetRenderTargetsDefault();
		for (auto& c : transparentMeshes) {
			setActiveMaterial(c.first);
			for (auto& m : c.second) {
				m.second->Draw(this, mV, mP, LVP);
			}
		}

		for (auto pps : ppscripts) {
			pps->PostProcess();
		}

		device->Begin2D();
		m_2dMutex.lock();
		for (auto& d : drawables) {
			d->Draw(device->GetWriteFactory(), device->GetDefFormat(), device->Get2DRenderTarget());
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
		device->SetActiveVSConstantBuffer(mat->GetBuffer());
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
		auto n = cameras.size();
		auto ar = GetAspectRatio();
		for (size_t i = 0; i < n; i++) {
			cameras[i]->SetAspectRatio(ar);
		}
	}

	void SceneRender::setActiveMaterial(SmartPtr<Material> pMat){
		device->SetActiveVSConstantBuffer(pMat->GetBuffer(), 1);
		device->SetActivePSConstantBuffer(pMat->GetBuffer());
		device->SetPSTextures(pMat->GetTextures());
		device->SetActiveShader(pMat->GetShader());
	}

	void SceneRender::updateMeshes() {
		//removeModels
		while (!removeQ.empty()) {
			auto mod = removeQ.front();
			removeQ.pop();
			if (mod.IsNull()) continue;
			auto n = mod->GetSubMeshesCount();
			for (size_t i = 0; i < n; i++) {
				for (auto& col : renderingMeshes) {
					if (mod.IsNull()) continue;
					auto m = mod->GetSubMesh(i);
					auto rm = col.second[m];
					if (!rm.IsNull())
						rm.Release();
					col.second.erase(m);
				}

				for (auto& col : transparentMeshes) {
					if (mod.IsNull()) continue;
					auto m = mod->GetSubMesh(i);
					auto rm = col.second[m];
					if (!rm.IsNull())
						rm.Release();
					col.second.erase(m);
				}
			}
		}

		//add Models
		while (!addQ.empty()) {
			auto ren = addQ.front().first;
			auto tr = addQ.front().second;
			addQ.pop();
			auto n = ren->GetModel()->GetSubMeshesCount();
			for (size_t i = 0; i < n; i++) {

				auto pMat = ren->GetMaterial(i);
				if (pMat.IsNull()) continue;

				auto mesh = ren->GetModel()->GetSubMesh(i);
				if (mesh->vertexBuffer.empty()) continue;
				auto rMesh = new RenderingMesh();
				rMesh->pVertexBuffer = device->CreateBuffer(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER);
				rMesh->pIndexBuffer = device->CreateBuffer(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER);
				rMesh->orig = mesh;

				rMesh->transform = tr;
				if (pMat->renderType == RenderType::Solid) {
					renderingMeshes[pMat].insert({ mesh, rMesh });
				} else if (pMat->renderType == RenderType::Transparent) {
					transparentMeshes[pMat].insert({ mesh, rMesh });
				}
			}
		}
	}

	void SceneRender::AddModel(ModelRender* ren, Transform* tr) {
		std::lock_guard lock(m_mutex);
		addQ.push({ ren,tr });
	}

	void SceneRender::RemoveModel(SmartPtr<Model> mod) {
		std::lock_guard lock(m_mutex);
		removeQ.push(mod);
	}

	void SceneRender::ChangeModel(ModelRender* ren, Transform* tr, SmartPtr<Model> mod) {
		std::lock_guard lock(m_mutex);
		removeQ.push(mod);
		addQ.push({ ren,tr });
	}

	bool SceneRender::UpdateBuffers(Mesh* mesh) {
		/*std::lock_guard lock(m_mutex);
		auto it = renderingMeshes.find(mesh);
		if (it == renderingMeshes.end()) {
			it = transparentMeshes.find(mesh);
			if (it == transparentMeshes.end())
				return false;
		}
		auto rMesh = it->second;
		rMesh->pIndexBuffer->Release();
		rMesh->pVertexBuffer->Release();
		rMesh->pIndexBuffer = device->CreateBuffer(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER);
		rMesh->pVertexBuffer = device->CreateBuffer(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER);
		return true;*/
		THROW_EXCEPTION("NOTIMPL");
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