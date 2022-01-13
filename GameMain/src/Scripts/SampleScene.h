#pragma once

#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Model.h>
#include <GameEngine\Components\UI\Text.h>
#include <GameEngine\Components\Rigidbody.h>

#include "CameraMoverScript.h"
#include "ChunkGenerator.h"
#include "UI\DebugText.h"
#include "PlayerHandler.h"

using namespace Game;

class SampleScene : public Scene {
	void Initialize() {
		auto debugText = Instaniate();
		debugText->SetName(L"DebugText");
		auto chunk = Instaniate();
		chunk->SetName(L"Chunk");

		auto player = Instaniate();
		player->SetName(L"Player");

		auto& ren = GetRender();

		SmartPtr<Camera> cam = new Camera();

		cam->SetFov(90);
		cam->SetClippingPlanes(.01, 1000.);
		cam->rotation = Quaternion::identity;
		cam->position = { 1, 1, 1 };

		ren.AddCamera(cam);

		auto sh = ren.CreateShader(L"Diffuse");

		sh->LoadVertexShader(Util::ReadFile(L"VertexShader.cso"));
		sh->LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

		sh = ren.CreateShader(L"Water");
		sh->usesDepthBuffer = true;

		sh->LoadVertexShader(Util::ReadFile(L"WaterVS.cso"));
		sh->LoadPixelShader(Util::ReadFile(L"WaterShader.cso"));

		auto chunkGen = new ChunkGenerator();
		auto camMover = new CameraMover();
		auto playerH = new PlayerHandler();

		camMover->chunkGen = chunkGen;
		playerH->chunkGen = chunkGen;

		//AddScript(camMover);
		chunk->AddComponent(chunkGen);

		player->AddComponent(new Transform());
		player->AddComponent(new Rigidbody());
		player->AddComponent(playerH);


		auto txt = new UI::Text();
		txt->SetText(L"SUS");
		txt->SetColor(D2D1::ColorF::White);
		debugText->AddComponent(txt);
		debugText->AddComponent(new DebugText());

		Scene::Initialize(); // call super initialize method to initialize sound manager with a valid camera reference
	}
};
