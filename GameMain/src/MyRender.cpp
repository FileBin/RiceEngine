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
	//XMFLOAT3 normal;
};

struct ConstantBuffer {
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct PixelConstantBuffer {
	XMFLOAT4 time;
};

MyRender::MyRender() {
	shader = nullptr;
	m_pVertexBuffer = nullptr;
}

void MyRender::loadPixelShaderFromFile(String FileName) {
	shader->LoadPixelShader(Util::ReadFile(FileName));
}

void MyRender::loadVertexShaderFromFile(String FileName, std::vector<D3D11_INPUT_ELEMENT_DESC> layout) {
	shader->LoadVertexShader(Util::ReadFile(FileName), layout);
}

bool MyRender::Init() {
	HRESULT hr = S_OK;

	shader = new Shader(device);

	m_inputLayout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12 + 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	try {
		loadVertexShaderFromFile(L"VertexShader.cso", m_inputLayout);
	} catch (HRESULT h) {
		hr = h;
	}
	if (FAILED(hr)) {
		Log::Err(L"���������� �������������� ���� \"%s\". ����������, ��������� ������ ��������� �� �����, ���������� ���� ����", L"VertexShader.cso");
		return false;
	}
	try {
		loadPixelShaderFromFile(L"PixelShader.cso");
	} catch (HRESULT h) {
		hr = h;
	}
	if (FAILED(hr)) {
		Log::Err(L"���������� �������������� ���� \"%s\". ����������, ��������� ������ ��������� �� �����, ���������� ���� ����", L"PixelShader.cso");
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

	// ��������� ���������� ������
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

	device->SetPrimitiveTopology();

	VSConstantBuffer = device->CreateBuffer<ConstantBuffer>({}, D3D11_BIND_CONSTANT_BUFFER);
	PSConstantBuffer = device->CreateBuffer<PixelConstantBuffer>({}, D3D11_BIND_CONSTANT_BUFFER);


	m_World = XMMatrixIdentity();


	XMVECTOR Eye = XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_View = XMMatrixLookAtLH(Eye, At, Up);

	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, GetAspectRatio(), 0.01f, 10000.0f);

	return true;
}

void MyRender::BeginFrame() {
	device->ClearFrame(Util::Color(.03, .1, .2, 1));
}

bool MyRender::Draw() {
	double time = .001 * clock();

	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(m_World * XMMatrixRotationY(time));
	cb.mView = XMMatrixTranspose(m_View);
	cb.mProjection = XMMatrixTranspose(m_Projection);
	device->LoadBufferSubresource(VSConstantBuffer, cb);

	PixelConstantBuffer pcb;
	pcb.time.x = time;
	device->LoadBufferSubresource(PSConstantBuffer, pcb);

	device->SetActiveVSConstantBuffer(VSConstantBuffer);
	device->SetActivePSConstantBuffer(PSConstantBuffer);

	device->SetActiveShader(*shader);

	device->Draw();

	return true;
}

void MyRender::Resize() {
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, GetAspectRatio(), 0.01f, 10000.0f);
}

void MyRender::Close() {
	_RELEASE(VSConstantBuffer);
	_RELEASE(m_pVertexBuffer);
	_RELEASE(m_pIndexBuffer);
}