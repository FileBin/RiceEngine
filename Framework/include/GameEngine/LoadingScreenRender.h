#pragma once

#include "RenderBase.h"

namespace Game {
	class LoadingScreenRender : public RenderBase {
	public:
		struct LoadingData {
			dbl percent;
			String label, stage;
		};

		virtual void BeginFrame() = 0;

		virtual bool Init() = 0;
		virtual bool Draw(LoadingData data) = 0;
		virtual void Close() = 0;
		virtual void Resize() = 0;

		bool Draw() { Draw({}); };
	};
}

