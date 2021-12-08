#include "MyRender.h"
#include <xnamath.h>
#include <D3Dcompiler.h>
#include <ios>
#include <filesystem>
#include <fstream>
#include <GameFramework/Log.h>
using namespace GameEngine;

struct SimpleVertex {
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

struct ConstantBuffer {
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

MyRender::MyRender() {
	shader = nullptr;
	m_pVertexBuffer = nullptr;
}

HRESULT MyRender::loadPixelShaderFromFile(std::wstring FileName) {
	HRESULT res;
	ID3D11PixelShader* PS;

	std::ifstream infile(FileName, std::ios_base::binary);
	auto buffer = std::vector<char>(std::istreambuf_iterator<char>(infile),
		std::istreambuf_iterator<char>());
	auto n = buffer.size();

	auto data = std::vector<byte>(n);

	for (size_t i = 0; i < n; i++) {
		data[i] = static_cast<byte>(buffer[i]);
	}

	shader->LoadPixelShader(data);

	return S_OK;
}

HRESULT MyRender::loadVertexShaderFromFile(std::wstring FileName, std::vector<D3D11_INPUT_ELEMENT_DESC> layout) {
	HRESULT res;
	ID3D11VertexShader* VS;

	std::ifstream infile(FileName, std::ios_base::binary);
	auto buffer = std::vector<char>(std::istreambuf_iterator<char>(infile),
		std::istreambuf_iterator<char>());
	auto n = buffer.size();

	auto data = std::vector<byte>(n);

	for (size_t i = 0; i < n; i++) {
		data[i] = static_cast<byte>(buffer[i]);
	}

	shader->LoadVertexShader(data, layout);

	return S_OK;
}

bool MyRender::Init() {
	HRESULT hr = S_OK;

	shader = new Shader(device);

	m_inputLayout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = loadVertexShaderFromFile(L"VertexShader.cso", m_inputLayout);
	if (FAILED(hr)) {
		Log::Err(L"Невозможно скомпилировать файл \"%s\". Пожалуйста, запустите данную программу из папки, содержащей этот файл", L"VertexShader.cso");
		return false;
	}

	hr = loadPixelShaderFromFile(L"PixelShader.cso");
	if (FAILED(hr)) {
		Log::Err(L"Невозможно скомпилировать файл \"%s\". Пожалуйста, запустите данную программу из папки, содержащей этот файл", L"PixelShader.cso");
		return false;
	}

	std::vector<SimpleVertex> vertices =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) }
	};

	m_pVertexBuffer = device->CreateBuffer(vertices, D3D11_BIND_VERTEX_BUFFER);

	// Установка вершинного буфера
	device->SetActiveVertexBuffer<SimpleVertex>(m_pVertexBuffer);
	
	std::vector<UINT> indices =
	{
		3,1,0,
		2,1,3,

		0,5,4,
		1,5,0,

		3,4,7,
		0,4,3,

		1,6,5,
		2,6,1,

		2,7,6,
		3,7,2,

		6,4,5,
		7,4,6,
	};


	m_pIndexBuffer = device->CreateBuffer<UINT>(indices, D3D11_BIND_INDEX_BUFFER);

	device->SetActiveIndexBuffer(m_pIndexBuffer);

	// установка топологии примитива
	device->SetPrimitiveTopology();

	m_pConstantBuffer = device->CreateBuffer<ConstantBuffer>({}, D3D11_BIND_CONSTANT_BUFFER);

	m_World = XMMatrixIdentity();


	XMVECTOR Eye = XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//m_View = XMMatrixIdentity();
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	float width = 640.0f;
	float height = 480.0f;
	//m_Projection = XMMatrixIdentity();
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / height, 0.01f, 10000.0f);

	return true;
}

void MyRender::BeginFrame() {
	device->ClearFrame(Color(.03, .1, .2, 1));
}

bool MyRender::Draw() {
	//m_View = XMMatrixTranslation(-1, sin(clock() * .001), .5);
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(m_World * XMMatrixRotationY(.001 * clock()));
	cb.mView = XMMatrixTranspose(m_View);
	cb.mProjection = XMMatrixTranspose(m_Projection);
	device->LoadBufferSubresource(m_pConstantBuffer, cb);

	device->SetActiveConstantBuffer(m_pConstantBuffer);

	device->SetActiveShader(*shader);

	//m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	device->Draw();
	//m_pImmediateContext->DrawIndexed(36, 0, 0);

	return true;
}

void MyRender::Close() {
	_RELEASE(m_pConstantBuffer);
	_RELEASE(m_pVertexBuffer);
	_RELEASE(m_pIndexBuffer);
}