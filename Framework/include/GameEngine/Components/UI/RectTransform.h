#pragma once

#include "../Transform.h"

namespace Game::UI {
	class RectTransform : public Transform {
	public:
		enum class Anchor {
			TopLeft, TopCenter, TopRight,
			MiddleLeft, MiddleCenter, MiddleRight,
			BottomLeft, BottomCenter, BottomRight
		} anchor;

		Vector2 GetAnchor() {
			switch (anchor) {
			case Anchor::TopLeft:
				return Vector2(-1, -1);
			case Anchor::TopCenter:
				return Vector2(0, -1);
			case Anchor::TopRight:
				return Vector2(1, -1);
			case Anchor::MiddleLeft:
				return Vector2(-1, 0);
			case Anchor::MiddleCenter:
				return Vector2(0, 0);
			case Anchor::MiddleRight:
				return Vector2(1, 0);
			case Anchor::BottomLeft:
				return Vector2(-1, 1);
			case Anchor::BottomCenter:
				return Vector2(0, 1);
			case Anchor::BottomRight:
				return Vector2(1, 1);
			}
			THROW_INVALID_ARG_EXCEPTION("RectTransform::anchor");
		}

		Vector2 TransformPoint(Vector2 vec) {
			auto vec3 = Vector3(vec.x, vec.y, 1) * GetTransformationMatrix();
			return { vec3.x, vec3.y };
		}

		Vector2 TransformVector(Vector2 vec) {
			auto vec3 = Vector3(vec.x, vec.y, 0) * GetTransformationMatrix();
			return { vec3.x, vec3.y };
		}

		Vector2 GetScale2D() {
			auto scale = GetScale();
			return { scale.x, scale.y };
		}

		Vector2 GetPosition2D() {
			auto pos = GetPosition();
			return { pos.x, pos.y };
		}

		Vector2 GetPosition2DWithAnchor(Canvas* canvas) {
			auto& ren = GetSceneObject().GetScene().GetRender();
			auto pos = GetPosition2D();

			auto anchor = GetAnchor();

			//transform to normalized coords
			anchor += Vector2::one;
			anchor *= .5;

			anchor.x *= canvas->referenceResoluton.x;
			anchor.y *= canvas->referenceResoluton.y;

			pos += anchor;
			return pos;
		}
	};
}
