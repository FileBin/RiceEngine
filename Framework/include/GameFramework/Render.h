#pragma once
#include "stdafx.h"
#include "Device.h"
#include "Math.h"

namespace GameEngine {
	class Render {
	public:
		Render();
		virtual ~Render();

		virtual void BeginFrame() = 0;
		void EndFrame();
		void Shutdown();
		void SetDevice(Device* device);

		Vector2 GetViewportSize();

		double GetAspectRatio();

		void* operator new(size_t i) {
			return _aligned_malloc(i, 16);
		}

		void operator delete(void* p) {
			_aligned_free(p);
		}

		virtual bool Init() = 0;
		virtual bool Draw() = 0;
		virtual void Close() = 0;
		virtual void Resize() = 0;

	protected:
		Device* device;
	};
}

