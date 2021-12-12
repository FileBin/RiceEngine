#pragma once
#include <GameEngine/RenderBase.h>
#include <GameEngine/macros.h>
#include <vector>
#include <xnamath.h>
#include <GameEngine/InputManager.h>

using namespace Game;

class MyRender : public RenderBase {
public:
	MyRender();
	bool Init();
	void BeginFrame();
	bool Draw();
	void Close();
	void Resize();

private:
	void loadPixelShaderFromFile(String FileName);
	void loadVertexShaderFromFile(String FileName, VertexLayout layout);

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

