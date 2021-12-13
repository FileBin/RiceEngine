#pragma once

#include <GameEngine\ScriptBase.h>
#include <MyRender.h>
#include <GameEngine\Log.h>
#include <GameEngine\ModelRender.h>

#include "MsaaTestScript.h"
#include "InitScript.h"
#include "PostInitScript.h"

using namespace Game;

class InitScript : public ScriptBase {
	void Run() {
		auto& engine = GetEngine();

		auto model = new Model();
		auto mesh = Mesh<Vertex>();

		mesh.layout = Vertex::GetLayout();

		mesh.vertexBuffer = {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), },//{0,0,0}, {0,0,0,0}, {0,0}},
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), },//{0,0,0}, {0,0,0,0}, {0,0}},
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), },//{0,0,0}, {0,0,0,0}, {0,0}},
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), },//{0,0,0}, {0,0,0,0}, {0,0}},
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), },//{0,0,0}, {0,0,0,0}, {0,0}},
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), },//{0,0,0}, {0,0,0,0}, {0,0}},
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), },//{0,0,0}, {0,0,0,0}, {0,0}},
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), },//{0,0,0}, {0,0,0,0}, {0,0}},
		};

		mesh.indexBuffer = {
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

		model->setSubMeshesCount(1);
		model->SetSubMesh(mesh, 0);

		auto* render = new ModelRenderer();

		render->SetModel(*model);

		engine.SetRender(render);
		//engine.SetRender(new MyRender());

		auto* postInit = new PostInitScript();
		postInit->render = render;

		engine.RegisterScript(postInit, Stage::PostInit);
		engine.RegisterScript(new MsaaTestScript());
	}
};
