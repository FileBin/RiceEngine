#pragma once

#include "../../Vectors/Vector2.h"
namespace Game::UI {
	class Canvas {
	public:
		Vector2 referenceResoluton;
		Vector2 currentResolution;

		Vector2 TransformPositionToScreen(Vector2 pos) {
			pos /= referenceResoluton;
			return pos * currentResolution;
		}

		Vector2 TransformPositionToView(Vector2 pos) {
			pos /= referenceResoluton;
			return pos;
		}

		Vector2 TransformScaleToScreen(Vector2 scale) {
			auto cur = currentResolution;
			cur.x = max(cur.x, referenceResoluton.x);
			cur.y = max(cur.y, referenceResoluton.y);
			scale /= max(referenceResoluton.x, referenceResoluton.y);
			return scale * min(cur.x, cur.y);
		}

		Vector2 TransformScaleToView(Vector2 scale) {
			scale /= max(referenceResoluton.x, referenceResoluton.y);
			return scale;
		}

		dbl TransformScaleToScreen(dbl scale) {
			auto cur = currentResolution;
			cur.x = max(cur.x, referenceResoluton.x);
			cur.y = max(cur.y, referenceResoluton.y);
			scale /= max(referenceResoluton.x, referenceResoluton.y);
			return scale * min(cur.x, cur.y);
		}
	};
}
