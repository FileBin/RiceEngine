#pragma once

#include "../stdafx.h"

#include "../Scene/IRenderable.h"
#include "../Model.h"
#include "..\Components\Transform.h"
#include "../Device.h"

namespace Game {
	class RenderingMesh : public IRenderable {
	public:
		std::mutex renderMutex;

		SmartPtr<Transform> transform;
		SmartPtr<Mesh> orig;
		SmartPtr<Material> mat;
		SmartPtr<Device> device;
		Microsoft::WRL::ComPtr<Buffer> pConstBuffer = nullptr;
		Microsoft::WRL::ComPtr<Buffer> pIndexBuffer = nullptr;
		Microsoft::WRL::ComPtr<Buffer> pVertexBuffer = nullptr;

		void Render(Matrix4x4 View, Matrix4x4 Projection, Matrix4x4 LVP) {
			std::lock_guard l(renderMutex);
			if (pIndexBuffer.Get() == nullptr || pVertexBuffer.Get() == nullptr) return;
			ConstantBufferData cb = {};
			auto World = transform->GetTransformationMatrix();
			cb.World = World;
			cb.WorldView = cb.World * View;
			cb.Projection = Projection;
			cb.LightWVP = World * LVP;

			if (orig.IsNull() || !orig->CheckVisiblity(cb)) return;

			device->LoadBufferSubresource(pConstBuffer.Get(), cb);
			device->SetActiveVSConstantBuffer(pConstBuffer.Get());
			setActiveMaterial(mat);

			device->SetActiveVertexBuffer<Vertex>(pVertexBuffer.Get());
			device->SetActiveIndexBuffer(pIndexBuffer.Get());
			device->Draw();
		}

		bool IsTransparent() {
			return mat->renderType == RenderType::Transparent;
		}

	private:
		void setActiveMaterial(SmartPtr<Material> pMat) {
			device->SetActiveVSConstantBuffer(pMat->GetBuffer(), 1);
			device->SetActivePSConstantBuffer(pMat->GetBuffer());
			device->SetPSTextures(pMat->GetTextures());
			device->SetActiveShader(pMat->GetShader());
		}
	};
}