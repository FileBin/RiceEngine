#pragma once

#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Model.h>
#include <GameEngine\Components\UI\Text.h>

#include "CameraMoverScript.h"
#include "ChunkGenerator.h"
#include "UI\DebugText.h"

using namespace Game;

class SampleScene : public Scene {
	void Initialize() {
		auto debugText = Instaniate();
		debugText->SetName(L"DebugText");
		auto chunk = Instaniate();
		chunk->SetName(L"Chunk");

		auto& ren = GetRender();

		auto cam = std::make_shared<Camera>();

		cam->SetFov(90);
		cam->SetClippingPlanes(.01, 1000.);
		cam->rotation = Quaternion::identity;
		cam->position = { 100000, 30, -5000 };

		ren.AddCamera(cam);

		auto sh = ren.CreateShader(L"Diffuse");

		sh->LoadVertexShader(Util::ReadFile(L"VertexShader.cso"));
		sh->LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

		sh = ren.CreateShader(L"Water");
		sh->usesDepthBuffer = true;

		sh->LoadVertexShader(Util::ReadFile(L"WaterVS.cso"));
		sh->LoadPixelShader(Util::ReadFile(L"WaterShader.cso"));

		AddScript(new CameraMover());
		chunk->AddComponent(new ChunkGenerator());


		auto txt = new UI::Text();
		txt->SetText(L"SUS");
		txt->SetColor(D2D1::ColorF::White);
		debugText->AddComponent(txt);
		debugText->AddComponent(new DebugText());

		Scene::Initialize(); // call super initialize method to initialize sound manager with a valid camera reference
	}
};
