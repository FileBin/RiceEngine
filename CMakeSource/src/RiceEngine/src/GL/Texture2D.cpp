#include "pch.h"

#include <Rice/GL/Texture2D.hpp>

NSP_GL_BEGIN

Texture2D::Texture2D(String filename, ID3D11Device* device, D3D11_TEXTURE_ADDRESS_MODE u_mode, D3D11_TEXTURE_ADDRESS_MODE v_mode, D3D11_FILTER filter) {
	using namespace Util;
	HRESULT hr = S_OK;
	ppResource = new ID3D11ShaderResourceView * [1];
	hr = D3DX11CreateShaderResourceViewFromFileW(device, filename.c_str(), nullptr, nullptr, ppResource, &hr);
	ThrowIfFailed(hr);
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = filter;
	sampDesc.AddressU = u_mode;
	sampDesc.AddressV = v_mode;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = 1;
	//sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampDesc, &pSampler);
	ThrowIfFailed(hr);
}

Texture2D::Texture2D(ID3D11ShaderResourceView** ppShaderResource, ID3D11Device* device) {
	using namespace Util;
	HRESULT hr = S_OK;
	ppResource = ppShaderResource;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampDesc, &pSampler);
	ThrowIfFailed(hr);
}

Texture2D::~Texture2D() {
	//_RELEASE(*ppResource);
	//_RELEASE(pSampler);
}

NSP_GL_END