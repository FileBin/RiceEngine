#pragma once
#include <GameEngine\ScriptBase.h>
#include <MyRender.h>
#include <GameEngine\Log.h>
#include <GameEngine\ModelRender.h>

#include "InitScript.h"
#include "CameraMover.h"

using namespace Game;

class PostInitScript : public ScriptBase {
public:
	ModelRenderer* render;

	void Run() {
		auto& ren = *render;
		auto& en = GetEngine();
		auto& sh = en.CreateShader();
		sh.LoadVertexShader(Util::ReadFile(L"VertexShader.cso"), Vertex::GetLayout());
		sh.LoadPixelShader(Util::ReadFile(L"DiffuseShader.cso"));
		auto& mat = en.CreateMaterial(sh);
		ren.SetMaterial(0, mat);
		ren.camera.SetFov(90);
		ren.camera.transform.pos.y = 2;
		ren.camera.transform.pos.z = -5;
		ren.camera.transform.scale = { 1,1,1 };
		ren.transform.scale = { 1,1,1 };

		auto s = new CameraMover();
		s->render = render;

		en.RegisterScript(s);
	}
};