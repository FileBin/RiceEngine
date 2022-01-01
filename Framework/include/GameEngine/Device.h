#pragma once
#include "stdafx.h"
#include "Util.h"
#include "Shader.h"

namespace Game {
	using namespace Game::Util;

	class Device {
	public:
		Device();
		~Device();

		void Draw2D();

		void Initialize(HWND hwnd, size_t videoCardIdx = 0);

		template<typename T>
		ID3D11Buffer* CreateBuffer(std::vector<T> data = {},
			UINT bindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE,
			UINT CpuAccessFlags = 0,
			D3D11_USAGE usage = D3D11_USAGE_DEFAULT) {

			ID3D11Buffer* buf = nullptr;
			ThrowIfFailed(_createBuffer((void*)data.data(), data.size(), sizeof(T), &buf, usage, bindFlags, CpuAccessFlags));
			return buf;
		}

		ID3D11Buffer* CreateBuffer(data_t data, UINT stride,
			UINT bindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE,
			UINT CpuAccessFlags = 0,
			D3D11_USAGE usage = D3D11_USAGE_DEFAULT) {
			ID3D11Buffer* buf = nullptr;
			ThrowIfFailed(_createBuffer((void*)data.data(), data.size() / stride, stride, &buf, usage, bindFlags, CpuAccessFlags));
			return buf;
		}

		template<typename T>
		void UpdateBufferData(ID3D11Buffer* buf, std::vector<T> data = {}) {
			ThrowIfFailed(_updateBuffer(data.data(), data.size(), sizeof(T), buf));
		}

		template<typename T>
		void LoadBufferSubresource(ID3D11Buffer* buffer, T &resource, size_t idx = 0) {
			context->UpdateSubresource(buffer, idx, nullptr, &resource, 0, 0);
		}

		template<typename T>
		void SetActiveVertexBuffer(ID3D11Buffer* buffer) {
			ThrowIfFailed(_loadVertexBuffer(&buffer, sizeof(T)));
		}

		void SetActiveIndexBuffer(ID3D11Buffer* buffer, DXGI_FORMAT textFormat = DXGI_FORMAT_R32_UINT);
		void SetActiveVSConstantBuffer(ID3D11Buffer* buffer, size_t index = 0);
		void SetActivePSConstantBuffer(ID3D11Buffer* buffer, size_t index = 0);

		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		void SetActiveShader(Shader &shader);

		void SetBlendState(bool transparent);

		ID3D11PixelShader* CreatePixelShader(data_t shaderData);

		ID3D11VertexShader* CreateVertexShader(data_t shaderData);

		ID3D11InputLayout* CreateInputLayout(VertexLayout layout, data_t shaderData);

		void Draw();

		void ClearFrame(Color color = Color(0,0,0));
		void SwapBuffers();

		void Resize();
		void ReCreateSwapChain() { ReCreateSwapChain(Util::GetWindowScreenSize(hwnd)); }
		void ReCreateSwapChain(Vector2 screenSize);

		HWND GetHWND() { return hwnd; }
		void SetMsaa(int level) { msaaLevel = level; }
		size_t GetMsaaLevel() const { return msaaLevel; }

		bool IsInitialized() { return initialized; }

	private:
		bool initialized = false;
		HWND hwnd = 0;
		size_t indexCount = 0;
		size_t msaaLevel = 1;
		IDXGIFactory* factory = nullptr;
		IDXGIAdapter* adapter = nullptr;
		ID3D11Device* device = nullptr;
		IDXGISwapChain* swapChain = nullptr;
		ID3D11DeviceContext* context = nullptr;
		ID3D11RenderTargetView* renderTarget = nullptr;
		ID3D11DepthStencilView* depthStencil = nullptr;
		ID3D11Texture2D* depthStencilTex = nullptr;
		ID3D11RasterizerState* state = nullptr;
		ID3D11DepthStencilState* pDSState;
		ID3D11BlendState* transparentState, *solidState;
		ID3D11Texture2D* textureFor2d = nullptr;


		DirectX::SpriteBatch* spriteBatch = nullptr;
		std::unordered_map<String, DirectX::SpriteFont*> fonts{};

		ID2D1RenderTarget* renderTarget2d = nullptr;
		ID2D1Factory* factory2d = nullptr;
		ID2D1SolidColorBrush* defBrush = nullptr;
		IDWriteTextFormat* textFormat = nullptr;
		IDWriteFactory* writeFactory = nullptr;
		IDWriteTextLayout* layout = nullptr;

		D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_NULL;
		D3D_FEATURE_LEVEL featureLvl = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

		HRESULT _createBuffer(void* pData, size_t nData, size_t stride, ID3D11Buffer** outbuf, D3D11_USAGE usage, UINT bindFlags, UINT cpuFlags);
		HRESULT _updateBuffer(void* pData, size_t nData, size_t stride, ID3D11Buffer* buf);
		HRESULT _loadVertexBuffer(ID3D11Buffer** buffer, size_t stride);
		void _init(size_t adapterIdx = 0);
		void _createDepthStencil(Vector2 size);
		void _create2d();
		void _create2dRT();
	};
}