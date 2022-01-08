#pragma once

#include <GameEngine/Components/MonoScript.h>
#include <GameEngine/Components/UI/Text.h>
#include <GameEngine/Scene/Scene.h>
#include <GameEngine/Camera.h>

using namespace Game;

class DebugText : public MonoScript {
public:
	UI::Text* txt = nullptr;
	Vector3* playerPos = nullptr;

	void Start() {
		txt = GetSceneObject().GetComponents<UI::Text>()[0];
		auto& ren = GetRender();
		playerPos = &ren.GetActiveCamera()->position;
	}

	void Update() {
		txt->SetText(std::format(L"X: {:.2f}\nY: {:.2f}\nZ: {:.2f}", playerPos->x, playerPos->y, playerPos->z));
	}
};

