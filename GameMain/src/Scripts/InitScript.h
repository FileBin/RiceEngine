#pragma once

#include <GameEngine\ScriptBase.h>
#include <MyRender.h>
#include <GameEngine\Log.h>
#include <GameEngine\ModelRender.h>

#include "MsaaTestScript.h"
#include "PostInitScript.h"

using namespace Game;

class InitScript : public ScriptBase {
	void Run() {
		auto& engine = GetEngine();

		auto model = new Model();
		auto mesh = Mesh();

		mesh.layout = Vertex::GetLayout();

		mesh.vertexBuffer = {
			{ {-1.0f, 1.0f, -1.0f}, { 0.0f, 1.0f, 0.0f}, },
			{ {1.0f, 1.0f, -1.0f}, { 0.0f, 1.0f, 0.0f}, },
			{ {1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f, 0.0f}, },
			{ {-1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f, 0.0f}, },

			{ {-1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f, 0.0f}, },
			{ {1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f, 0.0f}, },
			{ {1.0f, -1.0f, 1.0f}, { 0.0f, -1.0f, 0.0f}, },
			{ {-1.0f, -1.0f, 1.0f}, { 0.0f, -1.0f, 0.0f}, },

			{ {-1.0f, -1.0f, 1.0f}, { -1.0f, 0.0f, 0.0f}, },
			{ {-1.0f, -1.0f, -1.0f}, { -1.0f, 0.0f, 0.0f}, },
			{ {-1.0f, 1.0f, -1.0f}, { -1.0f, 0.0f, 0.0f}, },
			{ {-1.0f, 1.0f, 1.0f}, { -1.0f, 0.0f, 0.0f}, },

			{ {1.0f, -1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}, },
			{ {1.0f, -1.0f, -1.0f}, { 1.0f, 0.0f, 0.0f}, },
			{ {1.0f, 1.0f, -1.0f}, { 1.0f, 0.0f, 0.0f}, },
			{ {1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}, },

			{ {-1.0f, -1.0f, -1.0f}, { 0.0f, 0.0f, -1.0f}, },
			{ {1.0f, -1.0f, -1.0f}, { 0.0f, 0.0f, -1.0f}, },
			{ {1.0f, 1.0f, -1.0f}, { 0.0f, 0.0f, -1.0f}, },
			{ {-1.0f, 1.0f, -1.0f}, { 0.0f, 0.0f, -1.0f}, },

			{ {-1.0f, -1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f }, },
			{ {1.0f, -1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f }, },
			{ {1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f }, },
			{ {-1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f }, },
		};

		mesh.indexBuffer = {
			3,1,0,
			2,1,3,

			6,4,5,
			7,4,6,

			11,9,8,
			10,9,11,

			14,12,13,
			15,12,14,

			19,17,16,
			18,17,19,

			22,20,21,
			23,20,22
		};

		model->SetSubMeshesCount(1);
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
