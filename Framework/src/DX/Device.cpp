#include "pch.h"
#include <GameEngine/Device.h>
#include <GameEngine/macros.h>
#include <GameEngine/Vectors.h>
#include <GameEngine/Log.h>
#include <GameEngine/Util.h>
#include <GameEngine/Util/exception.h>

namespace Game {

	Device::Device() {
	}
	Device::~Device() {
		_RELEASE(renderTarget);
		_RELEASE(context);
		_RELEASE(swapChain);
		_RELEASE(device);
	}

	void Device::_create2dRT() {
		if (renderTarget2d != nullptr) return;
		IDXGISurface1* pBackBuffer = NULL;
		ThrowIfFailed(swapChain->GetBuffer(
			0,
			__uuidof(IDXGISurface1),
			reinterpret_cast<void**>(&pBackBuffer)
		));

		IDXGISurface1* surf;
		ThrowIfFailed(pBackBuffer->QueryInterface(&surf));

		D2D1_RENDER_TARGET_PROPERTIES rtDesc = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE::D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
		);

		ThrowIfFailed(factory2d->CreateDxgiSurfaceRenderTarget(surf, &rtDesc, &renderTarget2d));

		renderTarget2d->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&defBrush
		);
	}

	void Device::_create2d() {
		auto options = D2D1_FACTORY_OPTIONS();
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &factory2d);


		DWriteCreateFactory(DWRITE_FACTORY_TYPE::DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&writeFactory));

		writeFactory->CreateTextFormat(
			L"Arial",
			nullptr,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			20.0f * 96.0f / 72.0f,
			L"en-US",
			&textFormat
		);
	}

	void Device::Draw2D() {
		using namespace D2D1;
		renderTarget2d->BeginDraw();


		ThrowIfFailed(writeFactory->CreateTextLayout(L"SUS Амогус", 11, textFormat, 100, 100, &layout));

		renderTarget2d->DrawTextLayout({ 0,0 }, layout, defBrush);

		layout->Release();
		layout = nullptr;

		renderTarget2d->EndDraw();


		/*using DirectX::XMFLOAT2;
		using namespace DirectX;
		spriteBatch->Begin();
		for (auto pair : fonts) {
			pair.second->DrawString(spriteBatch, L"SUS", XMFLOAT2(0,0), Colors::WhiteSmoke, 0, XMFLOAT2(0,0), XMFLOAT2(1,1));
		}
		spriteBatch->End();*/
	}

#pragma region Initialize
	void Device::_init(size_t adapterIdx) {
		Vector2 size = Util::GetWindowScreenSize(hwnd);

		UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
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

		DXGI_ADAPTER_DESC adapterDesc;
		adapter->GetDesc(&adapterDesc);
		Log::log(Log::NONE, L"\nVideocard Info:\n Description: {}\n VideoMemory {}M\n", adapterDesc.Description,
			adapterDesc.DedicatedVideoMemory / 0x100000);

		ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &device, &featureLvl, &context));
		_create2d();
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
		
		rsdesc.AntialiasedLineEnable = true;

		ThrowIfFailed(device->CreateRasterizerState(&rsdesc, &state));

		//enable blending
		D3D11_BLEND_DESC omDesc;
		ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
		omDesc.RenderTarget[0].BlendEnable = true;

		omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		ThrowIfFailed(device->CreateBlendState(&omDesc, &transparentState));

		//solidState
		ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
		omDesc.RenderTarget[0].BlendEnable = false;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		ThrowIfFailed(device->CreateBlendState(&omDesc, &solidState));
	}

	void Device::SetBlendState(bool transparent) {
		if (transparent) {
			context->OMSetBlendState(transparentState, 0, 0xffffffff);
		} else {
			context->OMSetBlendState(solidState, 0, 0xffffffff);
		}
	}

	void Device::Initialize(HWND hwnd, size_t idx) {
		this->hwnd = hwnd;		
		_init(idx);
		initialized = true;
	}
#pragma endregion

	HRESULT Device::_createBuffer(void* pData, size_t nData, size_t stride, ID3D11Buffer** buf, D3D11_USAGE usage, UINT bFlags, UINT cFlags) {
		auto hr = S_OK;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		if (cFlags == 0) {
			bd.Usage = D3D11_USAGE_DEFAULT;
		} else {
			bd.Usage = D3D11_USAGE_DYNAMIC;
		}
		if (pData == nullptr) {
			bd.ByteWidth = stride;
		} else {
			bd.ByteWidth = stride * nData;
		}
		bd.BindFlags = bFlags;
		bd.CPUAccessFlags = cFlags;

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

	HRESULT Device::_updateBuffer(void* pData, size_t nData, size_t stride, ID3D11Buffer* buf) {
		D3D11_MAPPED_SUBRESOURCE resource;
		context->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, pData, nData * stride);
		context->Unmap(buf, 0);
		return S_OK;
	}

	HRESULT Device::_loadVertexBuffer(ID3D11Buffer** pBuffer, size_t stride) {
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, pBuffer, (UINT*)&stride, &offset);
		return S_OK;
	}

	void Device::_createDepthStencil(Vector2 size) {
		D3D11_TEXTURE2D_DESC dsDesc;
		dsDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		dsDesc.SampleDesc.Count = msaaLevel;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.MipLevels = 1;
		dsDesc.ArraySize = 1;
		dsDesc.Width = lround(size.x);
		dsDesc.Height = lround(size.y);
		dsDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		dsDesc.Usage = D3D11_USAGE_DEFAULT;
		dsDesc.CPUAccessFlags = 0;
		dsDesc.MiscFlags = 0;

		ThrowIfFailed(device->CreateTexture2D(&dsDesc, 0, &depthStencilTex));

		void* p = (void*)depthStencil;
		_RELEASE(depthStencil);

		ThrowIfFailed(device->CreateDepthStencilView(depthStencilTex, nullptr, &depthStencil));
		_RELEASE(depthStencilTex);

		D3D11_DEPTH_STENCIL_DESC Desc;

		// Depth test parameters
		Desc.DepthEnable = true;
		Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		Desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;

		// Stencil test parameters
		Desc.StencilEnable = true;
		Desc.StencilReadMask = 0xFF;
		Desc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		Desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		Desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		Desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		Desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		// Stencil operations if pixel is back-facing
		Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		Desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR;
		Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		Desc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		// Create depth stencil state
		device->CreateDepthStencilState(&Desc, &pDSState);
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

		_RELEASE(swapChain);

		ThrowIfFailed(factory->CreateSwapChain(device, &sd, &swapChain));

		ID3D11Texture2D* pBackBuffer = NULL;
		ThrowIfFailed(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));

		_RELEASE(renderTarget);
		ThrowIfFailed(device->CreateRenderTargetView(pBackBuffer, NULL, &renderTarget));
		_RELEASE(pBackBuffer);

		_createDepthStencil(screenSize);
		_create2dRT();
	}

	void Device::SetActiveIndexBuffer(ID3D11Buffer* buffer, DXGI_FORMAT textFormat) {
		D3D11_BUFFER_DESC desc;
		(*buffer).GetDesc(&desc);

		UINT stride = 0;

		switch (textFormat) {
		case DXGI_FORMAT_UNKNOWN:
			throw Game::exception("Unknown Format!", 260, L"Device.cpp : void Device::SetActiveIndexBuffer(ID3D11Buffer* buffer, DXGI_FORMAT textFormat)");
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
			throw Game::exception("Format Error!", 272, L"Device.cpp : void Device::SetActiveIndexBuffer(ID3D11Buffer* buffer, DXGI_FORMAT textFormat)");
			break;
		}

		indexCount = desc.ByteWidth / stride;
		context->IASetIndexBuffer(buffer, textFormat, 0);
	}

	void Game::Device::SetActiveVSConstantBuffer(ID3D11Buffer* buffer, size_t index) {
		ID3D11Buffer* arr[] = { buffer };
		context->VSSetConstantBuffers(index, 1, arr);
	}

	void Game::Device::SetActivePSConstantBuffer(ID3D11Buffer* buffer, size_t index) {
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

	ID3D11InputLayout* Device::CreateInputLayout(VertexLayout layout, data_t shaderData) {
		ID3D11InputLayout* l;
		ThrowIfFailed(device->CreateInputLayout(layout.data(), layout.size(), shaderData.data(), shaderData.size(), &l));
		return l;
	}

	void Device::Draw() {
		context->OMSetDepthStencilState(pDSState, 1);
		context->RSSetState(state);
		context->DrawIndexed(indexCount, 0, 0);
	}

	void Device::ClearFrame(Color color) {
		context->OMSetRenderTargets(1, &renderTarget, depthStencil);
		float c[] = { color.r,color.g,color.b,color.A };
		context->ClearRenderTargetView(renderTarget, c);
		context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	void Device::SwapBuffers() {
		swapChain->Present(0, 0);
	}

	void Device::Resize() {
		context->OMSetRenderTargets(0, 0, 0);

		_RELEASE(depthStencil)
		_RELEASE(renderTarget)
		_RELEASE(renderTarget2d)


		Vector2 size;
		RECT rect;
		GetClientRect(hwnd, &rect);
		size.x = rect.right - rect.left;
		size.y = rect.bottom - rect.top;
		if (size.SqrLength() < 1) {
			size = { 640, 480 };
		}
		ReCreateSwapChain(size);
		D3D11_VIEWPORT vp;
		vp.Width = size.x;
		vp.Height = size.y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		context->RSSetViewports(1, &vp);
	}

}