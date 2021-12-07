#pragma once
#include <Render.h>
#include <macros.h>
#include <vector>
#include <xnamath.h>
#include <Input/InputManager.h>

using namespace GameFramework;

class MyRender : public Render {
public:
	MyRender();
	bool Init(HWND hwnd);
	bool Draw();
	void Close();

	void* operator new(size_t i) {
		return _aligned_malloc(i, 16);
	}

	void operator delete(void* p) {
		_aligned_free(p);
	}

private:
	HRESULT loadPixelShaderFromFile(std::wstring FileName);
	HRESULT loadVertexShaderFromFile(std::wstring FileName);

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputLayout;

	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Projection;

	ID3D11Buffer* m_pIndexBuffer;
	ID3D11Buffer* m_pConstantBuffer;
};

