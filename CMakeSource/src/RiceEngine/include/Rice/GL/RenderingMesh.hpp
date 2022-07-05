#pragma once

#include "../stdafx.hpp"

#include "../Scene/IRenderable.hpp"
#include "Model.hpp"
#include "GraphicsManager.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "UniformBuffer.hpp"

#include "../Components/Transform.hpp"

NSP_GL_BEGIN

class RenderingMesh : public IRenderable
{
public:
	std::mutex renderMutex;

	uint idx = std::numeric_limits<uint>::max();

	RefPtr<Transform> transform;
	RefPtr<Mesh> orig;
	RefPtr<Material> mat;
	RefPtr<Device> device;
	pUniformBuffer constBuffer = nullptr;
	pIndexBuffer indexBuffer = nullptr;
	pVertexBuffer vertexBuffer = nullptr;

	void Register(size_t i) { idx = i; }

	size_t Unregister()
	{
		auto i = idx;
		idx = SIZE_T_MAX;
		return i;
	}

	void Render(Matrix4x4 View, Matrix4x4 Projection, Matrix4x4 LVP)
	{
		std::lock_guard l(renderMutex);
		if (pIndexBuffer.Get() == nullptr || pVertexBuffer.Get() == nullptr)
			return;
		ConstantBufferData cb = {};
		auto World = transform->GetTransformationMatrix();
		cb.World = World;
		cb.WorldView = cb.World * View;
		cb.Projection = Projection;
		cb.LightWVP = World * LVP;

		if (orig.IsNull() || !orig->CheckVisiblity(cb))
			return;

		device->LoadBufferSubresource(pConstBuffer.Get(), cb);
		device->SetActiveVSConstantBuffer(pConstBuffer.Get());
		setActiveMaterial(mat);

		device->SetActiveVertexBuffer<Vertex>(pVertexBuffer.Get());
		device->SetActiveIndexBuffer(pIndexBuffer.Get());
		device->Draw();
	}

	void RenderTransparent(Matrix4x4 View, Matrix4x4 Projection, Matrix4x4 LVP)
	{
		Render(View, Projection, LVP);
	}

	bool IsTransparent()
	{
		return mat->renderType == RenderType::Transparent;
	}

private:
	void setActiveMaterial(SmartPtr<Material> pMat)
	{
		device->SetActiveVSConstantBuffer(pMat->GetBuffer(), 1);
		device->SetActivePSConstantBuffer(pMat->GetBuffer());
		device->SetPSTextures(pMat->GetTextures());
		device->SetActiveShader(pMat->GetShader());
	}
};

NSP_GL_END
