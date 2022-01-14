#include "pch.h"
#include <GameEngine\SceneRender.h>
#include <GameEngine\Material.h>
#include <GameEngine\Model.h>
#include <GameEngine\Camera.h>
#include <GameEngine\Util\exceptions.h>
#include <queue>
#include <GameEngine\Components\UI\IDrawable.h>

namespace Game {

	bool SceneRender::Init() {
		constantBuffer = device->CreateBuffer<ConstantBufferData>({}, D3D11_BIND_CONSTANT_BUFFER);

		return true;
	}

	void Game::SceneRender::SetupSkybox(SmartPtr<Material> skyboxMat) {
		skyBox = new RenderingMesh();

		auto mesh = CreateSkyBoxMesh();

		skyBox->pVertexBuffer = device->CreateBuffer(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER);
		skyBox->pIndexBuffer = device->CreateBuffer(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER);
		skyBox->orig = mesh;
		skyBox->pMat = skyboxMaterial = skyboxMat;
		skyBox->pPos = new Vector3();
		skyBox->pRot = new Quaternion();
		skyBox->pScale = new Vector3(1, 1, 1);
	}

	void SceneRender::BeginFrame() {
		device->ClearFrame({ 0.1f, 0.15f, 0.6f, 1.f });
		if (!skyBox.IsNull()) {
			auto cam = cameras[activeCameraIdx];
			skyBox->Draw(this, Matrix4x4::Rotation(cam->rotation.Opposite()), cam->GetProjectionMatrix(), false);
			device->ClearZBuffer();
		}
	}

	void SceneRender::RenderingMesh::Draw(SceneRender* ren, Matrix4x4f View, Matrix4x4f Projection, bool check) {
		auto device = ren->device;
		auto constantBuffer = ren->constantBuffer.Get();
		ConstantBufferData cb = {};
		cb.World = Matrix4x4::TRS(*pPos, *pRot, *pScale); // TODO: values must be getted from the transform
		cb.View = View;
		cb.Projection = Projection;

		if (check)
			if (orig.IsNull() || !orig->CheckVisiblity(cb)) return;

		device->LoadBufferSubresource(constantBuffer, cb);
		device->SetActiveVSConstantBuffer(constantBuffer);
		device->SetActiveVSConstantBuffer(pMat->GetBuffer(), 1);

		device->SetActiveVertexBuffer<Vertex>(pVertexBuffer.Get());
		device->SetActiveIndexBuffer(pIndexBuffer.Get());
		device->SetActivePSConstantBuffer(pMat->GetBuffer());
		device->SetActiveShader(pMat->GetShader());
		device->SetPSTextures(pMat->GetTextures());

		device->Draw();
	}

	bool SceneRender::Draw() {
		auto cam = *cameras[activeCameraIdx];
		device->SetPrimitiveTopology();
		device->SetBlendState(false);

		Matrix4x4f mV = cam.GetTransformationMatrix();
		Matrix4x4f mP = cam.GetProjectionMatrix();
		m_mutex.lock();
		for (auto& pair : renderingMeshes) {
			pair.second->Draw(this, mV, mP);
		}

		device->SetBlendState(true);
		device->UnsetDepthBuffer();
		for (auto& pair : transparentMeshes) {
			pair.second->Draw(this, mV, mP);
		}
		m_mutex.unlock();

		device->Begin2D();
		m_2dMutex.lock();
		for (auto& d : drawables) {
			d->Draw(device->GetWriteFactory(), device->GetDefFormat(), device->Get2DRenderTarget());
		}
		m_2dMutex.unlock();
		device->End2D();

		return true;
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

	void SceneRender::AddModel(SmartPtr<Model> model, std::vector<SmartPtr<Material>> materials) {
		auto n = model->GetSubMeshesCount();
		for (size_t i = 0; i < n; i++) {
			auto mesh = model->GetSubMesh(i);
			if (mesh->vertexBuffer.empty()) continue;
			auto rMesh = new RenderingMesh();
			rMesh->pVertexBuffer = device->CreateBuffer(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
			rMesh->pIndexBuffer = device->CreateBuffer(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
			rMesh->orig = mesh;

			rMesh->pPos = model->pPos;
			rMesh->pRot = model->pRot;
			rMesh->pScale = model->pScale;

			rMesh->pMat = materials[i];

			std::lock_guard lock(m_mutex);

			switch (rMesh->pMat->renderType) {
			case RenderType::Solid:
				renderingMeshes[mesh] = rMesh;
				break;
			case RenderType::Transparent:
				transparentMeshes[mesh] = rMesh;
				break;
			default:
				THROW_INVALID_ARG_EXCEPTION("material->rederType");
				break;
			}
		}
	}

	void SceneRender::RemoveModel(SmartPtr<Model> model) {
		auto n = model->GetSubMeshesCount();
		for (size_t i = 0; i < n; i++) {
			auto mesh = model->GetSubMesh(i);
			std::lock_guard lock(m_mutex);
			auto it = renderingMeshes.find(mesh);
			if (it != renderingMeshes.end()) {
				it->second.Release();
				renderingMeshes.erase(it);
				continue;
			} 
			it = transparentMeshes.find(mesh);
			if (it != transparentMeshes.end()) {
				it->second.Release();
				transparentMeshes.erase(it);
			}
		}
	}

	void SceneRender::ChangeModel(SmartPtr<Model> removemodel, SmartPtr<Model> replace, std::vector<SmartPtr<Material>> materials) {
		std::lock_guard lock(m_mutex);
		auto n = removemodel->GetSubMeshesCount();
		for (size_t i = 0; i < n; i++) {
			auto mesh = removemodel->GetSubMesh(i);
			auto it = renderingMeshes.find(mesh);
			if (it != renderingMeshes.end()) {
				it->second.Release();
				renderingMeshes.erase(it);
				continue;
			}
			it = transparentMeshes.find(mesh);
			if (it != transparentMeshes.end()) {
				it->second.Release();
				transparentMeshes.erase(it);
			}
		}
		n = replace->GetSubMeshesCount();
		for (size_t i = 0; i < n; i++) {
			auto mesh = replace->GetSubMesh(i);
			if (mesh->vertexBuffer.empty()) continue;
			auto rMesh = new RenderingMesh();
			rMesh->pVertexBuffer = device->CreateBuffer(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER);
			rMesh->pIndexBuffer = device->CreateBuffer(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER);
			rMesh->orig = mesh;

			rMesh->pPos = replace->pPos;
			rMesh->pRot = replace->pRot;
			rMesh->pScale = replace->pScale;

			rMesh->pMat = materials[i];

			switch (rMesh->pMat->renderType) {
			case RenderType::Solid:
				renderingMeshes[mesh] = rMesh;
				break;
			case RenderType::Transparent:
				transparentMeshes[mesh] = rMesh;
				break;
			default:
				THROW_INVALID_ARG_EXCEPTION("material->rederType");
				break;
			}
		}
	}

	bool SceneRender::UpdateBuffers(SmartPtr<Mesh> mesh) {
		std::lock_guard lock(m_mutex);
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
		return true;
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

		Vector2f texcoords[4] = {
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