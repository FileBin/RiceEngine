#pragma once

#include "stdafx.h"

#include "LoadingScreenRender.h"
#include "Components\UI\Text.h"
#include "Scene\SceneObject.h"
namespace Game {
	class DefaultLoadingScreenRender : public LoadingScreenRenderBase {
		SceneObject* txt;
		UI::Text* text;
		UI::Canvas canvas;
		class LoadingScene : public Scene {
			DefaultLoadingScreenRender* ren;
		public:
			LoadingScene(DefaultLoadingScreenRender* render) : Scene() {
				ren = render;
			}
			void Init() {
				auto& sceneRen = GetRender();
				sceneRen.AddCamera(new Camera());
				ren->canvas.currentResolution = Util::GetWindowScreenSize(ren->device->GetHWND());
				ren->canvas.referenceResoluton = { 800,600 };
				ren->txt = this->Instaniate();
				ren->text = new UI::Text();
				ren->text->SetCanvas(&ren->canvas);
				ren->text->SetText(L"Loading...");
				ren->text->SetColor(D2D1::ColorF::White);
				auto rectTransform = new UI::RectTransform();
				rectTransform->SetPosition({ 0,0 });
				rectTransform->SetScale({ ren->canvas.referenceResoluton.x, ren->canvas.referenceResoluton.y });
				ren->txt->AddComponent(ren->text);
				ren->txt->AddComponent(rectTransform);

				ren->text->Enable();
			}
			void InitResourceManager() {
				_initResourceManager(L"resources/MainMenuSceneResources.json");
			}
		} *scene;
	public:
		virtual bool Init() {
			scene = new LoadingScene(this);
			scene->PreInit(engine, device);
			scene->Init();
			scene->PostInit();
			return true;
		}
		virtual void BeginFrame() {
			device->ClearFrame(D2D1::ColorF::Black);
		};
		virtual bool Draw(LoadingData data) {
			device->SetPrimitiveTopology();
			device->SetVPDefault();
			device->SetRenderTargetsDefault();
			device->SetRSState();

			device->Begin2D();
			text->Draw(device);
			device->End2D();
			return true;
		}
		virtual void Close() {
			_DELETE(text);
		}
		virtual void Resize() {
			canvas.currentResolution = Util::GetWindowScreenSize(device->GetHWND());
		}
	};
}