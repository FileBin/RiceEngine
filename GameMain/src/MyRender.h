#pragma once
#include <GameFramework/Render.h>
#include <GameFramework/macros.h>
#include <vector>
#include <xnamath.h>
#include <GameFramework/InputManager.h>

using namespace GameEngine;

class MyRender : public Render {
public:
	MyRender();
	bool Init();
	virtual void BeginFrame();
	bool Draw();
	void Close();
	void Resize();

private:
	void loadPixelShaderFromFile(String FileName);
	void loadVertexShaderFromFile(String FileName, std::vector<D3D11_INPUT_ELEMENT_DESC> layout);

	ID3D11Buffer* m_pVertexBuffer;
	Shader* shader;
	std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputLayout;

	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Projection;

	ID3D11Buffer* m_pIndexBuffer;
	ID3D11Buffer* VSConstantBuffer;
	ID3D11Buffer* PSConstantBuffer;
};

