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
#include "SamplePostProcess.h"

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

		auto diff = ren.CreateShader(L"Diffuse");

		diff->LoadVertexShader(Util::ReadFile(L"VertexShader.cso"));
		diff->LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));

		auto sh = ren.CreateShader(L"Water");
		sh->usesDepthBuffer = true;

		sh->LoadVertexShader(Util::ReadFile(L"WaterVS.cso"));
		sh->LoadPixelShader(Util::ReadFile(L"WaterShader.cso"));

		auto postsh = ren.CreateShader(L"PostProcess");
		postsh->usesDepthBuffer = true;

		postsh->LoadVertexShader(Util::ReadFile(L"PostProcessVS.cso"));
		postsh->LoadPixelShader(Util::ReadFile(L"SamplePostProcess.cso"));

		auto postMat = ren.CreateMaterial(L"PostMaterial", postsh, {});

		postMat->AddTexture(&ren.GetRenderTargetTex());
		postMat->AddTexture(&ren.GetDepthBufferTex());

		auto UnderWaterSh = ren.CreateShader(L"Underwater");

		UnderWaterSh->LoadVertexShader(Util::ReadFile(L"PostProcessVS.cso"));
		UnderWaterSh->LoadPixelShader(Util::ReadFile(L"Underwater.cso"));

	    postMat = ren.CreateMaterial(L"Underwater", UnderWaterSh, {});

		postMat->AddTexture(&ren.GetRenderTargetTex());
		postMat->AddTexture(&ren.GetDepthBufferTex());

		auto skyShader = ren.CreateShader(L"SkyShader");

		skyShader->LoadVertexShader(Util::ReadFile(L"SkyBoxVertex.cso"));
		skyShader->LoadPixelShader(Util::ReadFile(L"SkyBoxPixel.cso"));

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

		auto skymat = ren.CreateMaterial(L"SkyboxMaterial", skyShader, {});

		auto& skyTex = ren.CreateTexture(L"img/sky_finger.png");

		skymat->AddTexture(&skyTex);

		skymat->UpdateBuffer();

		ren.SetupSkybox(skymat);

		auto txt = new UI::Text();
		txt->SetText(L"SUS");
		txt->SetColor(D2D1::ColorF::White);
		debugText->AddComponent(txt);
		debugText->AddComponent(new DebugText());

		auto postProcessScript = new SamplePostProcess();
		postProcessScript->generator = chunkGen;
		AddScript(postProcessScript);
	}
};
