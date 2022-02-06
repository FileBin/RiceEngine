#pragma once

#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Model.h>
#include <GameEngine\Components\UI\Text.h>
#include <GameEngine\Components\UI\Button.h>
#include <GameEngine\Components\Rigidbody.h>

#include "../Scripts/CameraMoverScript.h"
#include "../Scripts/ChunkGenerator.h"
#include "../Scripts/UI/DebugText.h"
#include "../Scripts/PlayerHandler.h"
#include "../Scripts/SamplePostProcess.h"

using namespace Game;

class SampleScene : public Scene {
	void Init() {
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
		//auto camMover = new CameraMover();
		auto playerH = new PlayerHandler();

		//camMover->chunkGen = chunkGen;
		playerH->chunkGen = chunkGen;

		//AddScript(camMover);
		chunk->AddComponent(chunkGen);

		player->AddComponent(new Transform());
		player->AddComponent(new Rigidbody());
		player->AddComponent(playerH);

		/*auto sphere = player->Instantiate();
		auto sphereTransform = new Transform();
		sphereTransform->SetPosition({1,-1,3});
		sphere->AddComponent(sphereTransform);
		auto mr = new ModelRender();
		sphere->AddComponent(mr);

		auto sphereModel = new Model();
		sphereModel->SetSubMeshesCount(1);
		sphereModel->SetSubMesh(MeshGenerator::GenerateMesh([](Vector3 p) { return Util::sdSphere(p, 1.); }, { 8,8,8 }), 0);

		mr->SetModel(sphereModel);
		auto sphereMat = ren.CreateMaterial(L"SphereMat", diff, { Var(L"time"), Var(L"color"), Var(L"egst") });

		sphereMat->SetVar<Vector4f>(L"color", {.4,.4,.4,1.});
		sphereMat->SetVar<Vector4f>(L"egst", { .3,0,0,0 });
		sphereMat->UpdateBuffer();

		mr->SetMaterial(sphereMat, 0);*/

		auto skymat = ren.CreateMaterial(L"SkyboxMaterial", skyShader, {});

		auto& skyTex = ren.CreateTexture(L"img/sky_finger.png");

		skymat->AddTexture(&skyTex);

		skymat->UpdateBuffer();

		ren.SetupSkybox(skymat);

		//text
		auto debugText = Instaniate();
		debugText->SetName(L"DebugText");

		auto txt = new UI::Text();
		txt->SetText(L"SUS");
		txt->SetColor(D2D1::ColorF::White);

		auto rTransform = new UI::RectTransform();
		rTransform->SetPosition({ 0, 0 });
		rTransform->SetScale({ 800,600 });

		debugText->AddComponent(rTransform);
		debugText->AddComponent(txt);
		debugText->AddComponent(new DebugText());

		auto button = Instaniate();
		auto cross = Instaniate();
		cross->SetName(L"Cross");
		button->SetName(L"Button");

		UI::Button* but = new UI::Button();
		but->SetImg(&ren.CreateTexture(L"img/amogus.png"));
		rTransform = new UI::RectTransform();
		rTransform->anchor = UI::RectTransform::Anchor::TopRight;
		rTransform->SetPosition({ 0, 0 });
		rTransform->SetScale({ 100, 100 });

		but->setOnClickListener([](UI::Button* but) -> void {
			Log::log(Log::LogLevel::INFO, L"click"); 
			InputManager::LockMouse();
		});

		but->setOnReleaseListener([](UI::Button* but) -> void {
			Log::log(Log::LogLevel::INFO, L"release"); 
			but->SetImg(&but->GetSceneObject().GetScene().GetRender().CreateTexture(L"img/amogus.png")); 
		});

		but->setOnHoverInListener([](UI::Button* but) -> void {
			Log::log(Log::LogLevel::INFO, L"hover in"); });

		but->setOnHoverOutListener([](UI::Button* but) -> void {
			Log::log(Log::LogLevel::INFO, L"hover out"); });

		button->AddComponent(rTransform);
		button->AddComponent(but);

		UI::Image* pic = new UI::Image();
		pic->SetImg(&ren.CreateTexture(L"img/cross.png"));
		rTransform = new UI::RectTransform();
		rTransform->anchor = UI::RectTransform::Anchor::MiddleCenter;
		rTransform->SetPosition({ 0, 0 });
		rTransform->SetScale({ 50, 50 });
		cross->AddComponent(rTransform);
		cross->AddComponent(pic);

		//postProcess
		auto postProcessScript = new SamplePostProcess();
		postProcessScript->generator = chunkGen;
		AddScript(postProcessScript);
	}
};
