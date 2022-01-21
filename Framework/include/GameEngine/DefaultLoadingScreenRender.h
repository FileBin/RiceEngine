#pragma once

#include "stdafx.h"

#include "LoadingScreenRender.h"
#include "Components\UI\Text.h"
namespace Game {
	class DefaultLoadingScreenRender : public LoadingScreenRenderBase {
		UI::Text* text;
		virtual bool Init() {
			text = new UI::Text();
			text->SetText(L"Loading...");
			text->SetColor(D2D1::ColorF::White);
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
			text->Draw(device->GetWriteFactory(), device->GetDefFormat(), device->Get2DRenderTarget());
			device->End2D();
			return true;
		}
		virtual void Close() {
			_DELETE(text);
		}
		virtual void Resize() {}
	};
}