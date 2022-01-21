#pragma once

#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Model.h>
#include <GameEngine\Components\UI\Text.h>
#include <GameEngine\Components\Rigidbody.h>
#include <GameEngine\Components\Collider.h>
#include <GameEngine\Components\CustomCollider.h>
#include <GameEngine\Components\MeshCollider.h>
#include <GameEngine\Util\DistanceEstimator.h>

#include "../Scripts/Util/MeshGenerator.h"

#include "..\Scripts\CameraMoverScript.h"
#include "..\Scripts\UI\DebugText.h"

using namespace Game;

class PhysicsScene : public Scene {
	void Init() {

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

		AddScript(new CameraMover());

		auto debugText = Instaniate();
		debugText->SetName(L"DebugText");

		//Plane Init
		auto plane = Instaniate();
		plane->SetName(L"Plane");
		auto planeTr = new Transform();
		Vector3 planeN;
		planeN = { 0,1,0 };
		planeN.Normalize();
		std::function<dbl(Vector3)> planeSd = [planeN](Vector3 p) { 
			return opIntersect({
				Vector3::Dot(p, planeN),
				10 - (p - Vector3(0,-9,0)).Length(),
				}); };
		//auto planeCol = new CustomCollider();

		auto planeMesh = MeshGenerator::GenerateMesh(planeSd, { 32,32,32 }, { -20,-20,-20 }, { 20,20,20 });

		auto planeCol = new MeshCollider();
		//planeCol->SetFunc(planeSd);
		auto mr = new ModelRender();
		plane->AddComponent(mr);
		plane->AddComponent(planeTr);
		plane->AddComponent(planeCol);

		auto model = new Model();
		model->SetSubMeshesCount(1);
		model->SetSubMesh(planeMesh, 0);

		planeCol->SetModel(model);


		auto mat = ren.CreateMaterial(L"PlaneMat", diff, { Var(L"time"), Var(L"color"), Var(L"egst") });
		mat->SetVar<Vector4f>(L"color", { 1.f, .4f, .6f, 1.f });
		mat->SetVar<Vector4f>(L"egst", { 0.3f, 1.9f, 0.4f, 1.f });
		mat->UpdateBuffer();

		mr->SetModel(model);
		mr->SetMaterial(mat, 0);

		//Txt Init
		auto txt = new UI::Text();
		txt->SetText(L"SUS");
		txt->SetColor(D2D1::ColorF::White);
		debugText->AddComponent(txt);
		debugText->AddComponent(new DebugText());


		Scene::Init(); // call super initialize method to initialize sound manager with a valid camera reference
	}
};
