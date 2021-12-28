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

		auto& ren = GetRender();

		auto cam = new Camera();

		cam->SetFov(90);
		cam->SetClippingPlanes(.01, 1000.);
		cam->transform.scale = { 1, 1, 1 };
		cam->transform.rotation = Quaternion::identity;
		cam->transform.pos = { 0, 30, -5 };
		
		ren.AddCamera(cam);

		auto sh = ren.CreateShader(L"Diffuse");

		sh->LoadVertexShader(Util::ReadFile(L"VertexShader.cso"));
		sh->LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

		AddScript(new CameraMover());
		chunk->AddComponent(new ChunkGenerator());
	}
};
