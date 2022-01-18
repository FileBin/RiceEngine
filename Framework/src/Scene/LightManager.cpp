#include "pch.h"

#include <GameEngine\SceneRender.h>
#include <GameEngine\DX\Texture2D.h>
#include <GameEngine\Model.h>

namespace Game {
	void SceneRender::LightManager::Init(SceneRender* ren, std::vector<dbl> atlasSizes, dbl shadowDist, size_t shadowMapSize) {
		auto d3dDevice = ren->device->GetD3dDevice();

		shadowMapRes = shadowMapSize;
		shadowDistance = shadowDist;
		sceneRender = ren;
		shadowMapSizes = atlasSizes;
		numCascades = atlasSizes.size();
		lightDirection = { 0,-5,0 };

		D3D11_TEXTURE2D_DESC dsDesc;
		dsDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		dsDesc.SampleDesc.Count = 1;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.MipLevels = 1;
		dsDesc.ArraySize = 1;
		dsDesc.Width = shadowMapSize;
		dsDesc.Height = shadowMapSize;
		dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		dsDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		dsDesc.CPUAccessFlags = 0;
		dsDesc.MiscFlags = 0;

		ThrowIfFailed(d3dDevice->CreateTexture2D(&dsDesc, 0, shadowTex.GetAddressOf()));


		D3D11_DEPTH_STENCIL_VIEW_DESC dwdesc;
		ZeroMemory(&dwdesc, sizeof(dwdesc));
		dwdesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		dwdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dwdesc.Texture2D.MipSlice = 0;

		ThrowIfFailed(d3dDevice->CreateDepthStencilView(shadowTex.Get(), &dwdesc, DSV.GetAddressOf()));

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		//setup the description of the shader resource view
		shaderResourceViewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		//create the shader resource view.
		ThrowIfFailed(d3dDevice->CreateShaderResourceView(shadowTex.Get(), &shaderResourceViewDesc, SRV.GetAddressOf()));

		shadowAtlas = new Texture2D(SRV.GetAddressOf(), d3dDevice);

		lightBuffer = ren->device->CreateBuffer<LightBuffer>({}, D3D11_BIND_CONSTANT_BUFFER);
	}

	void SceneRender::LightManager::RenderShadowMap(Vector3 playerPos) {
		auto context = sceneRender->device->GetContext();
		ID3D11RenderTargetView* pNullView = nullptr;
		context->OMSetRenderTargets(1, &pNullView, DSV.Get());
		context->ClearDepthStencilView(DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		//sceneRender->device->ClearZBuffer();

		Vector3 translation = -playerPos;
		Quaternion rotation = Quaternion::LookAt(Vector3::zero, lightDirection).Opposite();
		auto d = shadowDistance / 2;

		auto ViewMatrix = Matrix4x4::TRS(-playerPos, rotation, Vector3::one);

		LightBuffer cb;
		cb.LVP = ViewMatrix * Matrix4x4::Ortographic({ shadowMapSizes[0], shadowMapSizes[0], d });
		cb.ambient.x = cb.ambient.y = cb.ambient.z = .3f;
		cb.diffuse.x = cb.diffuse.y = cb.diffuse.z = cb.diffuse.w = cb.ambient.w = 1.f;

		sceneRender->device->LoadBufferSubresource(lightBuffer.Get(), cb);

		for (size_t i = 0; i < numCascades; i++) {
			auto size = shadowMapSizes[i];
			//auto ProjMatrix = Matrix4x4::identity;
			auto ProjMatrix = Matrix4x4::Ortographic({ size, size, d });

			D3D11_VIEWPORT vp;
			vp.TopLeftY = 0;
			vp.TopLeftX = shadowMapRes * i;
			vp.Height = vp.Width = shadowMapRes;
			vp.MinDepth = 0.f;
			vp.MaxDepth = 1.f;
			sceneRender->device->SetVP(vp);

			for (auto& pair : sceneRender->renderingMeshes) {
				pair.second->DrawShadow(sceneRender, ViewMatrix, ProjMatrix);
			}
		}
		context->OMSetRenderTargets(0, 0, 0);
	}

	void SceneRender::RenderingMesh::DrawShadow(SceneRender* ren, Matrix4x4f View, Matrix4x4f Projection) {
		auto device = ren->device;
		auto constantBuffer = ren->constantBuffer.Get();
		ConstantBufferData cb = {};
		cb.World = Matrix4x4::TRS(*pPos, *pRot, *pScale); // TODO: values must be getted from the transform
		cb.View = View;
		cb.Projection = Projection;
		device->SetPrimitiveTopology();
		device->LoadBufferSubresource(constantBuffer, cb);
		device->SetActiveVSConstantBuffer(constantBuffer);
		device->SetActiveVSConstantBuffer(pMat->GetBuffer(), 1);

		device->SetActiveVertexBuffer<Vertex>(pVertexBuffer.Get());
		device->SetActiveIndexBuffer(pIndexBuffer.Get());

		auto context = device->GetContext();
		const auto& shader = pMat->GetShader();
		context->VSSetShader(shader.vertexShader, 0, 0);
		context->IASetInputLayout(shader.layout);
		context->PSSetShader(0, 0, 0);

		device->Draw();
	}
}