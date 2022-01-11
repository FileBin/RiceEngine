#pragma once

#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Model.h>
#include <GameEngine\Components\UI\Text.h>
#include <GameEngine\Components\Rigidbody.h>
#include <GameEngine\Components\Collider.h>
#include "../Scripts/Util/MeshGenerator.h"

#include "..\Scripts\CameraMoverScript.h"
#include "..\Scripts\UI\DebugText.h"

using namespace Game;

class PhysicsScene : public Scene {
	void Initialize() {

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

		/*//Sphere Init
		auto sphere = Instaniate();
		sphere->SetName(L"Sphere");
		auto sphereTr = new Transform();
		auto sphereBody = new Rigidbody();
		sphereBody->SetVelocity({ 15,0,0 });
		sphere->AddComponent(sphereTr);
		sphere->AddComponent(sphereBody);
		sphereTr->position = { 0,0,7 };
		auto mr = new ModelRender();
		sphere->AddComponent(mr);

		auto model = new Model();
		model->SetSubMeshesCount(1);
		model->SetSubMesh(MeshGenerator::GenerateMesh([](Vector3 p) { return p.Length() - 1.; }), 0);

		auto mat = ren.CreateMaterial(L"SphereMat", diff, { Var(L"time"), Var(L"color"), Var(L"egst") });
		mat->SetVar<Vector4f>(L"color", { .6f, .6f, .5f, 1.f });
		mat->SetVar<Vector4f>(L"egst", { 0.3f, 2.f, 0.8f, 1.f });
		mat->UpdateBuffer();

		mr->SetModel(model);
		mr->SetMaterial(mat, 0);*/

		//Plane Init
		auto plane = Instaniate();
		plane->SetName(L"Plane");
		auto planeTr = new Transform();
		Vector3 planeN;
		planeN = { 0,1,0 };
		planeN.Normalize();
		std::function<dbl(Vector3)> planeSd = [planeN](Vector3 p) { return Math::Max(Vector3::Dot(p, planeN), 10 - p.Length()); };
		auto planeCol = new Collider(planeSd);
		auto mr = new ModelRender();
		plane->AddComponent(mr);
		plane->AddComponent(planeTr);
		plane->AddComponent(planeCol);

		auto model = new Model();
		model->SetSubMeshesCount(1);
		model->SetSubMesh(MeshGenerator::GenerateMesh(planeSd, { 100,100,100 }, { -50,-100,-100 }, { 50,50,50 }), 0);

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


		Scene::Initialize(); // call super initialize method to initialize sound manager with a valid camera reference
	}
};
