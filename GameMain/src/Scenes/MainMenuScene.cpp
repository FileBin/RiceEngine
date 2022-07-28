#include "pch.h"
#include "MainMenuScene.h"

#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Model.h>
#include <GameEngine\Components\UI\Text.h>
#include <GameEngine\Components\UI\Slider.h>
#include <GameEngine\Components\Rigidbody.h>
#include <GameEngine\Components\UI\TextButton.h>

#include "../Scripts/CameraMoverScript.h"
#include "../Scripts/ChunkGenerator.h"
#include "../Scripts/UI/DebugText.h"
#include "../Scripts/PlayerHandler.h"
#include "../Scripts/SamplePostProcess.h"
#include "SampleScene.h"

void MainMenuScene::InitResourceManager() {
	_initResourceManager(L"MainMenuSceneResources.json");
}

void MainMenuScene::Init() {
	auto& ren = GetRender();
	auto& en = GetEngine();
	auto device = en.GetDevice();

	auto& resManager = GetResourceManager();

	SmartPtr<Camera> cam = new Camera();

	cam->SetFov(90);
	cam->SetClippingPlanes(.01, 1000.);
	cam->rotation = Quaternion::identity;
	cam->position = { 1, 1, 1 };

	ren.AddCamera(cam);

	UI::RectTransform* rectTransform;

	//background
	rectTransform = new UI::RectTransform();
	rectTransform->anchor = UI::RectTransform::Anchor::MiddleCenter;
	rectTransform->SetPosition({ 0,0 });
	rectTransform->SetScale({ 1920, 1080 });

	UI::Image* bg = new UI::Image();
	bg->SetImg(&ren.CreateTexture(resManager.GetString("BackgroundImg"), D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP));

	auto BackgroundImg = Instaniate();
	BackgroundImg->SetName(L"Background");
	BackgroundImg->AddComponent(rectTransform);
	BackgroundImg->AddComponent(bg);

	//start button
	UI::TextButton* button = UI::TextButton::CreateNew(L"Button", D2D1::ColorF::Black);
	button->SetFont(L"BankGothic Md BT");
	button->SetImg(&ren.CreateTexture(resManager.GetString("StartButtonImg"), D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP));
	button->setOnClickListener([&en](UI::Button* sender) {
		en.LoadScene(new SampleScene());
		});

	rectTransform = new UI::RectTransform();
	rectTransform->anchor = UI::RectTransform::Anchor::MiddleCenter;
	rectTransform->SetPosition({ 0,-100 });
	rectTransform->SetScale({ 200,33 });

	auto StartButton = Instaniate();
	StartButton->SetName(L"StartButton");
	StartButton->AddComponent(rectTransform);
	StartButton->AddComponent(button);

	//slider
	UI::Slider* slider = new UI::Slider();
	slider->SetImg(&ren.CreateTexture(resManager.GetString("CrossTexture"), D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP));
	slider->SetBgImg(&ren.CreateTexture(resManager.GetString("SliderImg"), D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP));

	slider->setOnClickListener([this](UI::Slider* sender) {
		GetSoundManager().play_sound("test", 1, { 0,0,0 });
		});

	rectTransform = new UI::RectTransform();
	rectTransform->anchor = UI::RectTransform::Anchor::BottomCenter;
	rectTransform->SetPosition({ 0,-20 });
	rectTransform->SetScale({ 200,35 });

	auto Slider = Instaniate();
	Slider->SetName(L"Slider");
	Slider->AddComponent(rectTransform);
	Slider->AddComponent(slider);

	auto debugText = Instaniate();

	rectTransform = new UI::RectTransform();
	rectTransform->anchor = UI::RectTransform::Anchor::TopLeft;
	rectTransform->SetPosition({ 0,0 });
	rectTransform->SetScale({ 800,600 });

}