#include <GameFramework/Device.h>
#include <GameFramework/macros.h>
#include <GameFramework/Vectors.h>
#include <GameFramework/Util.h>

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
	HRESULT Device::Init(HWND hwnd) {
		HRESULT hr = S_OK;

		RECT rc;
		GetClientRect(hwnd, &rc);
		UINT width = rc.right - rc.left;
		UINT height = rc.bottom - rc.top;

		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		UINT numDriverTypes = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
			driverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
				&sd, &swapChain, &device, &featureLvl, &context);
			if (SUCCEEDED(hr))
				break;
		}
		if (FAILED(hr))
			return hr;

		ID3D11Texture2D* pBackBuffer = NULL;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr))
			return hr;

		hr = device->CreateRenderTargetView(pBackBuffer, NULL, &renderTarget);
		_RELEASE(pBackBuffer);
		if (FAILED(hr))
			return hr;

		context->OMSetRenderTargets(1, &renderTarget, NULL);

		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		context->RSSetViewports(1, &vp);

		return hr;
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

	void GameEngine::Device::SetActiveConstantBuffer(ID3D11Buffer* buffer, size_t index) {
		ID3D11Buffer* arr[] = { buffer };
		context->VSSetConstantBuffers(index, 1, arr);
	}

	void Device::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology) {
		context->IASetPrimitiveTopology(topology);
	}

	void Device::SetActiveShader(Shader &shader) {
		context->VSSetShader(shader.vertexShader, nullptr, 0);
		context->IASetInputLayout(shader.layout);
		context->PSSetShader(shader.pixelShader, nullptr, 0);
	}


	ID3D11PixelShader* Device::CreatePixelShader(std::vector<byte> shaderData) {
		ID3D11PixelShader* shader;
		ThrowIfFailed(device->CreatePixelShader(&shaderData[0], shaderData.size(), nullptr, &shader));
		return shader;
	}

	ID3D11VertexShader* Device::CreateVertexShader(std::vector<byte> shaderData) {
		ID3D11VertexShader* shader;
		ThrowIfFailed(device->CreateVertexShader(&shaderData[0], shaderData.size(), nullptr, &shader));
		return shader;
	}

	ID3D11InputLayout* Device::CreateInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC> layout, std::vector<byte> shaderData) {
		ID3D11InputLayout* l;
		ThrowIfFailed(device->CreateInputLayout(layout.data(), layout.size(), shaderData.data(), shaderData.size(), &l));
		return l;
	}

	void Device::Draw() {
		context->DrawIndexed(indexCount, 0, 0);
	}

	void Device::ClearFrame(Color color) {
		float c[] = { color.r,color.g,color.b,color.a };
		context->ClearRenderTargetView(renderTarget, c);
	}

	void Device::SwapBuffers() {
		swapChain->Present(0, 0);
	}

	void Device::Resize() {
		// ������� ���������, ���������� � �������� ����������� ����.
		context->OMSetRenderTargets(0, 0, 0);
		renderTarget->Release();
		renderTarget = nullptr;

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

			context->OMSetRenderTargets(1, &renderTarget, nullptr);

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