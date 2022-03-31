#include "pch.h"
#include <GameEngine\Components\Transform.h>
#include <GameEngine\Components\MultiModelRender.h>
#include <GameEngine\Model.h>
#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Scene.h>

using namespace std;

#define min _STD min

namespace Game {
	void MultiModelRender::OnInit() {
		device = GetSceneObject().GetScene().GetEngine().GetDevice();
	}

	void MultiModelRender::OnEnable() {
		auto& scene = GetSceneObject().GetScene();
		auto& ren = scene.GetRender();
		transform = GetSceneObject().GetComponents<Transform>()[0];

		ren.RegisterModel(this);

		enabled = true;
	}

	void MultiModelRender::OnDisable() {
		auto& ren = GetSceneObject().GetScene().GetRender();
		ren.UnregisterModel(this);
		enabled = false;
	}

	void Game::MultiModelRender::SetMaterials(std::vector<SmartPtr<Material>> materials) {
		lock_guard l(renderMutex);
		auto n = materials.size();

		meshes.resize(n);

		for (auto i = 0; i < n; i++) {
			meshes[i].material = materials[i];
		}
	}

	void MultiModelRender::RemoveModel(MultiModelRender::UUID idx){
		auto n = meshes.size();
		if (idx.keys.IsNull()) return;
		for (size_t i = 0; i < n; i++) {
			auto& coll = meshes[i].meshes;
			if (idx.keys.Get()[i] == SIZE_T_MAX) continue;
			auto pData = coll.GetElemAt(idx.keys.Get()[i]);
			lock_guard l(renderMutex);
			coll.Unregister(idx.keys.Get()[i]);
			pData.Release();
		}
		idx.keys.Release();
	}

	MultiModelRender::UUID MultiModelRender::AddModel(SmartPtr<Model> model, Matrix4x4 transformationMatrix) {

		MultiModelRender::UUID uuid;

		uuid.ptr = this;

		auto n = meshes.size();
		uuid.keys_count = n;
		uuid.keys = new size_t[n];

		for (size_t i = 0; i < n; i++) {
			auto& coll = meshes[i].meshes;

			auto mesh = model->GetSubMesh(i);
			uuid.keys.Get()[i] = SIZE_T_MAX;
			if (mesh->indexBuffer.empty()) continue;

			auto pData = new RenderData();


			lock_guard l(renderMutex);
			uuid.keys.Get()[i] = coll.Register(pData);
			setupRenderData(pData, mesh, transformationMatrix);
		}

		return uuid;
	}

	void MultiModelRender::SetModel(MultiModelRender::UUID uuid, SmartPtr<Model> model, Matrix4x4 transformationMatrix) {

		auto n = uuid.keys_count;

		for (size_t i = 0; i < n; i++) {
			auto& coll = meshes[i].meshes;


			auto mesh = model->GetSubMesh(i);
			if (mesh->indexBuffer.empty()) {
				if (uuid.keys.Get()[i] == SIZE_T_MAX) continue;
				lock_guard l(renderMutex);
				coll.Unregister(uuid.keys.Get()[i]);
				uuid.keys.Get()[i] = SIZE_T_MAX;
				continue;
			}
			if (uuid.keys.Get()[i] == SIZE_T_MAX) {
				lock_guard l(renderMutex);
				uuid.keys.Get()[i] = coll.Register(new RenderData());
			}

			auto pData = coll.GetElemAt(uuid.keys.Get()[i]);

			if (!pData.IsNull()) {
				lock_guard l(renderMutex);
				_RELEASE(pData->indexBuffer);
				_RELEASE(pData->vertexBuffer);
				pData->mesh = nullptr;
				setupRenderData(pData.Get(), mesh, transformationMatrix);
			} else {
				uuid.keys.Get()[i] = SIZE_T_MAX;
			}
		}
	}

	void MultiModelRender::Render(Matrix4x4& View, Matrix4x4& Projection, Matrix4x4& LVP, RenderType type) {
		ConstantBufferData cb = {};
		auto World = transform->GetTransformationMatrix();
		cb.World = World;
		cb.WorldView = cb.World * View;
		cb.Projection = Projection;
		cb.LightWVP = World * LVP;
		auto cBuffer = GetSceneObject().GetScene().GetRender().GetConstBuffer();

		device->LoadBufferSubresource(cBuffer, cb);

		std::lock_guard l(renderMutex);
		auto n = meshes.size();
		for (size_t i = 0; i < n; i++) {
			const auto& mesh = meshes[i];
			if (mesh.material.IsNull()) continue;
			if (mesh.material->renderType != type) continue;

			setActiveMaterial(mesh.material);
			auto coll = mesh.meshes.GetCollectionWithGaps();
			for (auto data : coll) {
				if (data.IsNull()) continue;
				//if (!data->mesh->CheckVisiblity(cb)) continue;
				if (data->vertexBuffer == nullptr) continue;
				if (data->indexBuffer == nullptr) continue;
				device->SetActiveVertexBuffer<Vertex>(data->vertexBuffer.Get());
				device->SetActiveIndexBuffer(data->indexBuffer.Get());

				device->Draw();
			}
		}
	}

	void MultiModelRender::Render(Matrix4x4 View, Matrix4x4 Projection, Matrix4x4 LVP) {
		Render(View, Projection, LVP, RenderType::Solid);
	}

	void MultiModelRender::RenderTransparent(Matrix4x4 View, Matrix4x4 Projection, Matrix4x4 LVP) {
		Render(View, Projection, LVP, RenderType::Transparent);
	}

	void MultiModelRender::setupRenderData(RenderData* data, SmartPtr<Mesh> mesh, Matrix4x4& matrix) {
		data->mesh = mesh;

		data->indexBuffer = device->CreateBuffer<UINT>(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER);

		auto m = mesh->vertexBuffer.size();

		auto VBCopy = std::vector<Vertex>(m);

		for (auto i = 0; i < m; i++) {
			VBCopy[i] = mesh->vertexBuffer[i];
			VBCopy[i].position = (Vector3)VBCopy[i].position * matrix;
		}

		data->vertexBuffer = device->CreateBuffer<Vertex>(VBCopy, D3D11_BIND_VERTEX_BUFFER);
	}

	void MultiModelRender::setActiveMaterial(SmartPtr<Material> pMat) {
		device->SetActiveVSConstantBuffer(pMat->GetBuffer(), 1);
		device->SetActivePSConstantBuffer(pMat->GetBuffer());
		device->SetPSTextures(pMat->GetTextures());
		device->SetActiveShader(pMat->GetShader());
	}

	void MultiModelRender::Register(size_t i) { idx = i; }

	size_t MultiModelRender::Unregister() {
		auto i = idx;
		idx = SIZE_T_MAX;
		return i;
	}

	bool MultiModelRender::UUID::IsActive() {
		return keys_count == 0;
	}
}