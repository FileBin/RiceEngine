#include "MyRender.h"
#include <xnamath.h>
#include <D3Dcompiler.h>
#include <ios>
#include <filesystem>
#include <fstream>
#include <GameFramework/Log.h>
using namespace GameFramework;

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
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pVertexLayout = nullptr;
	m_pVertexBuffer = nullptr;
}

HRESULT MyRender::loadPixelShaderFromFile(std::wstring FileName) {
	HRESULT res;
	ID3D11PixelShader* PS;

	std::ifstream infile(FileName, std::ios_base::binary);
	auto buffer = std::vector<char>(std::istreambuf_iterator<char>(infile),
		std::istreambuf_iterator<char>());

	res = m_pd3dDevice->CreatePixelShader(&buffer[0], buffer.size(), nullptr, &PS);
	if (FAILED(res))
		return res;
	m_pImmediateContext->PSSetShader(PS, 0, 0);
	return S_OK;
}

HRESULT MyRender::loadVertexShaderFromFile(std::wstring FileName) {
	HRESULT res;
	ID3D11VertexShader* VS;

	std::ifstream infile(FileName, std::ios_base::binary);
	auto buffer = std::vector<char>(std::istreambuf_iterator<char>(infile),
		std::istreambuf_iterator<char>());

	res = m_pd3dDevice->CreateVertexShader(&buffer[0], buffer.size(), nullptr, &VS);
	if (FAILED(res))
		return res;

	res = m_pd3dDevice->CreateInputLayout(m_inputLayout.data(), m_inputLayout.size(), &buffer[0], buffer.size(), &m_pVertexLayout);

	if (FAILED(res))
		return res;

	// установка входного формата
	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

	m_pImmediateContext->VSSetShader(VS, 0, 0);
	return S_OK;
}

bool MyRender::Init(HWND hwnd) {
	HRESULT hr = S_OK;

	m_inputLayout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = loadVertexShaderFromFile(L"VertexShader.cso");
	if (FAILED(hr)) {
		Log::Err(L"Невозможно скомпилировать файл \"%s\". Пожалуйста, запустите данную программу из папки, содержащей этот файл", L"VertexShader.cso");
		return false;
	}

	hr = loadPixelShaderFromFile(L"PixelShader.cso");
	if (FAILED(hr)) {
		Log::Err(L"Невозможно скомпилировать файл \"%s\". Пожалуйста, запустите данную программу из папки, содержащей этот файл", L"PixelShader.cso");
		return false;
	}

	SimpleVertex vertices[] =
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


	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA Data;
	ZeroMemory(&Data, sizeof(Data));
	Data.pSysMem = vertices;

	hr = m_pd3dDevice->CreateBuffer(&bd, &Data, &m_pVertexBuffer);
	if (FAILED(hr))
		return false;

	// Установка вершинного буфера
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	
	UINT indices[] =
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

	D3D11_SUBRESOURCE_DATA IndexData;
	ZeroMemory(&IndexData, sizeof(IndexData));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(UINT) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	IndexData.pSysMem = indices;
	hr = m_pd3dDevice->CreateBuffer(&bd, &IndexData, &m_pIndexBuffer);
	if (FAILED(hr))
		return false;

	m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// установка топологии примитива
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);

	m_World = XMMatrixIdentity();


	XMVECTOR Eye = XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_View = XMMatrixIdentity();
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	float width = 640.0f;
	float height = 480.0f;
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / height, 0.01f, 10000.0f);

	return true;
}

bool MyRender::Draw() {
	//m_View = XMMatrixTranslation(-1, sin(clock() * .001), .5);
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(m_World * XMMatrixRotationY(.001 * clock()));
	cb.mView = XMMatrixTranspose(m_View);
	cb.mProjection = XMMatrixTranspose(m_Projection);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->DrawIndexed(36, 0, 0);

	return true;
}

void MyRender::Close() {
	_RELEASE(m_pConstantBuffer);
	_RELEASE(m_pVertexBuffer);
	_RELEASE(m_pIndexBuffer);
	_RELEASE(m_pVertexLayout);
	_RELEASE(m_pVertexShader);
	_RELEASE(m_pPixelShader);
}