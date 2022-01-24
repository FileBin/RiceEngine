#pragma once

#include "stdafx.h"

#include "LoadingScreenRender.h"
#include "Components\UI\Text.h"
namespace Game {
	class DefaultLoadingScreenRender : public LoadingScreenRenderBase {
		SceneObject* root, *txt;
		UI::Text* text;
		UI::Canvas canvas;
		virtual bool Init() {
			canvas.currentResolution = Util::GetWindowScreenSize(device->GetHWND());
			canvas.referenceResoluton = { 800,600 };
			root = new SceneObject(nullptr);
			txt = root->Instantiate();
			text = new UI::Text();
			text->SetCanvas(&canvas);
			text->SetText(L"Loading...");
			text->SetColor(D2D1::ColorF::White);
			auto rectTransform = new UI::RectTransform();
			rectTransform->SetPosition({ 0,0 });
			rectTransform->SetScale({ canvas.referenceResoluton.x, canvas.referenceResoluton.y });
			txt->AddComponent(text);
			txt->AddComponent(rectTransform);

			text->Enable();
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