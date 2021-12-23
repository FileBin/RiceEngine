#pragma once

#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Model.h>

#include "CameraMoverScript.h"
#include "ChunkGenerator.h"

using namespace Game;

class SampleScene : public Scene {
	void Initialize() {
		auto chunk = Instaniate();
		chunk->SetName(L"Chunk");

		auto render = new ModelRender();
		chunk->AddComponent(render);

		auto mesh = new Mesh();

		mesh->layout = Vertex::GetLayout();

		mesh->vertexBuffer = {
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

		mesh->indexBuffer = {
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

		auto& shader = GetEngine().CreateShader();
		shader.LoadVertexShader(Util::ReadFile(L"VertexShader.cso"), Vertex::GetLayout());
		shader.LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

		auto& ren = GetRender();

		auto& mat = ren.CreateMaterial(&shader);

		auto model = new Model();
		model->SetSubMeshesCount(1);
		model->SetSubMesh(mesh, 0);

		render->SetModel(model);
		render->SetMaterial(&mat, 0);

		auto cam = new Camera();

		cam->SetFov(50);
		cam->SetClippingPlanes(.001, 1000.);
		cam->transform.scale = { 1, 1, 1 };
		cam->transform.rotation = Quaternion::identity;
		cam->transform.pos = { 0, 60, -5 };
		
		ren.AddCamera(cam);

		AddScript(new CameraMover());
		chunk->AddComponent(new ChunkGenerator());
	}
};
