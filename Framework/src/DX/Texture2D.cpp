#include "pch.h"

#include <GameEngine/DX/Texture2D.h>

namespace Game {
	Texture2D::Texture2D(String filename, ID3D11Device* device) {
		using namespace Util;
		HRESULT hr = S_OK;
		D3DX11CreateShaderResourceViewFromFileW(device, filename.c_str(), nullptr, nullptr, &pResource, &hr);
		ThrowIfFailed(hr);
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sampDesc, &pSampler);
		ThrowIfFailed(hr);
	}

	Texture2D::~Texture2D() {
		_RELEASE(pResource);
		_RELEASE(pSampler);
	}
}