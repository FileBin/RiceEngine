#include "../../stdafx.hpp"

class Canvas;
typedef SmartPtr<Canvas> pCnavas;

#pragma once

#include "../../Math.hpp"

NSP_UI_BEGIN

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
		scale = TransformScaleToView(scale);
		return scale * currentResolution;
	}

	Vector2 TransformScaleToView(Vector2 scale) {
		scale /= _STD max(referenceResoluton.x, referenceResoluton.y);
		auto asp = currentResolution.x / currentResolution.y;
		if (asp >= 1) {
			scale.x /= asp;
		} else {
			scale.y *= asp;
		}

		return scale;
	}

	dbl TransformScaleToScreen(dbl scale) {
		auto cur = currentResolution;
		scale /= _STD max(referenceResoluton.x, referenceResoluton.y);
		return scale * _STD min(cur.x, cur.y);
	}
};

NSP_UI_END
