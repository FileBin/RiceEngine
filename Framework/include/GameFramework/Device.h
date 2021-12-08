#pragma once
#include "stdafx.h"
#include "Util.h"
#include "Shader.h"

namespace GameEngine {

	class Device {
	public:
		Device();
		~Device();

		HRESULT Init(HWND hwnd);

		template<typename T>
		ID3D11Buffer* CreateBuffer(std::vector<T> data = {}, UINT flags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) {
			ID3D11Buffer* buf = nullptr;
			ThrowIfFailed(_createBuffer((void*)data.data(), data.size(), sizeof(T), &buf, usage, flags));
			return buf;
		}

		template<typename T>
		void LoadBufferSubresource(ID3D11Buffer* buffer, T &resource, size_t idx = 0) {
			context->UpdateSubresource(buffer, idx, nullptr, &resource, 0, 0);
		}

		template<typename T>
		void SetActiveVertexBuffer(ID3D11Buffer* buffer) {
			ThrowIfFailed(_loadVertexBuffer(&buffer, sizeof(T)));
		}

		void SetActiveIndexBuffer(ID3D11Buffer* buffer, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);
		void SetActiveConstantBuffer(ID3D11Buffer* buffer, size_t index = 0);

		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		void SetActiveShader(Shader &shader);

		ID3D11PixelShader* CreatePixelShader(std::vector<byte> shaderData);

		ID3D11VertexShader* CreateVertexShader(std::vector<byte> shaderData);

		ID3D11InputLayout* CreateInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC> layout, std::vector<byte> shaderData);

		void Draw();

		void ClearFrame(Color color = Color(0,0,0));
		void SwapBuffers();

		void Resize();

		HWND GetHWND() { return hwnd; }
	private:
		bool initialized = false;
		HWND hwnd = 0;
		size_t indexCount = 0;

		ID3D11Device* device = nullptr;
		IDXGISwapChain* swapChain = nullptr;
		ID3D11DeviceContext* context = nullptr;
		ID3D11RenderTargetView* renderTarget = nullptr;

		D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_NULL;
		D3D_FEATURE_LEVEL featureLvl = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

		HRESULT _createBuffer(void* pData, size_t nData, size_t stride, ID3D11Buffer** outbuf, D3D11_USAGE usage, UINT flags);
		HRESULT _loadVertexBuffer(ID3D11Buffer** buffer, size_t stride);
	};
}