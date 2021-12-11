#include <GameEngine/Device.h>
#include <GameEngine/macros.h>
#include <GameEngine/Vectors.h>
#include <GameEngine/Util.h>
#include <GameEngine/Log.h>

namespace GameEngine {

	Device::Device() {
	}
	Device::~Device() {
		_RELEASE(renderTarget);
		_RELEASE(context);
		_RELEASE(swapChain);
		_RELEASE(device);
	}

#pragma region Initialize
	void Device::_init(size_t adapterIdx) {
		Vector2 size = Util::GetWindowScreenSize(hwnd);

		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		ThrowIfFailed(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));
		ThrowIfFailed(factory->EnumAdapters(adapterIdx, &adapter));

#ifdef _DEBUG
		DXGI_ADAPTER_DESC adapterDesc;
		adapter->GetDesc(&adapterDesc);
		Log::Debug(L"AdapterInfo:\n\n Description: {}\n VideoMemory {}M", adapterDesc.Description, adapterDesc.DedicatedVideoMemory / 0x100000);
#endif

		ThrowIfFailed(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_UNKNOWN, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &device, &featureLvl, &context));

		ReCreateSwapChain();

		

		D3D11_VIEWPORT vp;
		vp.Width = size.x;
		vp.Height = size.y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		context->RSSetViewports(1, &vp);

		D3D11_RASTERIZER_DESC rsdesc;

		ZeroMemory(&rsdesc, sizeof(rsdesc));

		rsdesc.CullMode = D3D11_CULL_BACK;
		rsdesc.FillMode = D3D11_FILL_SOLID;
		
		ThrowIfFailed(device->CreateRasterizerState(&rsdesc, &state));
	}

	void Device::Initialize(HWND hwnd, size_t idx) {
		this->hwnd = hwnd;		
		_init(idx);
	}
#pragma endregion

	HRESULT Device::_createBuffer(void* pData, size_t nData, size_t stride, ID3D11Buffer** buf, D3D11_USAGE usage, UINT flags) {
		auto hr = S_OK;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		if (pData == nullptr) {
			bd.ByteWidth = stride;
		} else {
			bd.ByteWidth = stride * nData;
		}
		bd.BindFlags = flags;
		bd.CPUAccessFlags = 0;

		if (pData != nullptr) {
			D3D11_SUBRESOURCE_DATA Data;
			ZeroMemory(&Data, sizeof(Data));
			Data.pSysMem = pData;

			ThrowIfFailed(device->CreateBuffer(&bd, &Data, buf));
		} else {
			ThrowIfFailed(device->CreateBuffer(&bd, nullptr, buf));
		}

		return S_OK;
	}

	HRESULT Device::_loadVertexBuffer(ID3D11Buffer** pBuffer, size_t stride) {
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, pBuffer, (UINT*)&stride, &offset);
		return S_OK;
	}

	void Device::_createDepthStencil(Vector2 size) {
		D3D11_TEXTURE2D_DESC dsDesc;
		dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsDesc.SampleDesc.Count = msaaLevel;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.MipLevels = 1;
		dsDesc.ArraySize = 1;
		dsDesc.Width = lround(size.x);
		dsDesc.Height = lround(size.y);
		dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsDesc.Usage = D3D11_USAGE_DEFAULT;
		dsDesc.CPUAccessFlags = 0;
		dsDesc.MiscFlags = 0;

		ThrowIfFailed(device->CreateTexture2D(&dsDesc, 0, &depthStencilTex));

		void* p = (void*)depthStencil;
		_RELEASE(depthStencil);

		ThrowIfFailed(device->CreateDepthStencilView(depthStencilTex, nullptr, &depthStencil));
		_RELEASE(depthStencilTex);
	}

	void Device::ReCreateSwapChain(Vector2 screenSize) {
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = screenSize.x;
		sd.BufferDesc.Height = screenSize.y;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;

		sd.SampleDesc.Count = msaaLevel;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = true;

		void* pp = (void*)swapChain;
		_RELEASE(swapChain);

		ThrowIfFailed(factory->CreateSwapChain(device, &sd, &swapChain));

		ID3D11Texture2D* pBackBuffer = NULL;
		ThrowIfFailed(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));

		pp = (void*)renderTarget;
		_RELEASE(renderTarget);
		ThrowIfFailed(device->CreateRenderTargetView(pBackBuffer, NULL, &renderTarget));
		_RELEASE(pBackBuffer);

		_createDepthStencil(screenSize);
	}

	void Device::SetActiveIndexBuffer(ID3D11Buffer* buffer, DXGI_FORMAT format) {
		D3D11_BUFFER_DESC desc;
		(*buffer).GetDesc(&desc);

		UINT stride = 0;

		switch (format) {
		case DXGI_FORMAT_UNKNOWN:
			throw std::exception("Unknown Format!");
			break;
		case DXGI_FORMAT_R32_UINT:
			stride = 4;
			break;
		case DXGI_FORMAT_R16_UINT:
			stride = 2;
			break;
		case DXGI_FORMAT_R8_UINT:
			stride = 1;
			break;
		default:
			throw std::exception("Format Error!");
			break;
		}

		indexCount = desc.ByteWidth / stride;
		context->IASetIndexBuffer(buffer, format, 0);
	}

	void GameEngine::Device::SetActiveVSConstantBuffer(ID3D11Buffer* buffer, size_t index) {
		ID3D11Buffer* arr[] = { buffer };
		context->VSSetConstantBuffers(index, 1, arr);
	}

	void GameEngine::Device::SetActivePSConstantBuffer(ID3D11Buffer* buffer, size_t index) {
		ID3D11Buffer* arr[] = { buffer };
		context->PSSetConstantBuffers(index, 1, arr);
	}

	void Device::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology) {
		context->IASetPrimitiveTopology(topology);
	}

	void Device::SetActiveShader(Shader &shader) {
		context->VSSetShader(shader.vertexShader, nullptr, 0);
		context->IASetInputLayout(shader.layout);
		context->PSSetShader(shader.pixelShader, nullptr, 0);
	}


	ID3D11PixelShader* Device::CreatePixelShader(data_t shaderData) {
		ID3D11PixelShader* shader;
		ThrowIfFailed(device->CreatePixelShader(shaderData.data(), shaderData.size(), nullptr, &shader));
		return shader;
	}

	ID3D11VertexShader* Device::CreateVertexShader(data_t shaderData) {
		ID3D11VertexShader* shader;
		ThrowIfFailed(device->CreateVertexShader(shaderData.data(), shaderData.size(), nullptr, &shader));
		return shader;
	}

	ID3D11InputLayout* Device::CreateInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC> layout, data_t shaderData) {
		ID3D11InputLayout* l;
		ThrowIfFailed(device->CreateInputLayout(layout.data(), layout.size(), shaderData.data(), shaderData.size(), &l));
		return l;
	}

	void Device::Draw() {
		context->RSSetState(state);
		context->DrawIndexed(indexCount, 0, 0);
	}

	void Device::ClearFrame(Color color) {
		context->OMSetRenderTargets(1, &renderTarget, depthStencil);
		float c[] = { color.r,color.g,color.b,color.a };
		context->ClearRenderTargetView(renderTarget, c);
		context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	void Device::SwapBuffers() {
		swapChain->Present(0, 0);
	}

	void Device::Resize() {
		// Очистка контекста, связанного с размером предыдущего окна.
		context->OMSetRenderTargets(0, 0, 0);
		renderTarget->Release();
		renderTarget = nullptr;
		depthStencil->Release();
		depthStencil = nullptr;

		Vector2 size;
		RECT rect;
		GetClientRect(hwnd, &rect);
		size.x = rect.right - rect.left;
		size.y = rect.bottom - rect.top;

		if (swapChain != nullptr) {
			HRESULT hr = swapChain->ResizeBuffers(1, lround(size.x), lround(size.y), DXGI_FORMAT_B8G8R8A8_UNORM, 0);

			ID3D11Texture2D* pBuffer;
			hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
				(void**)&pBuffer);

			hr = device->CreateRenderTargetView(pBuffer, nullptr, &renderTarget);
			pBuffer->Release();

			_createDepthStencil(size);

			D3D11_VIEWPORT vp;
			vp.Width = size.x;
			vp.Height = size.y;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			context->RSSetViewports(1, &vp);

			if (FAILED(hr))
				throw hr;
		}
	}

}