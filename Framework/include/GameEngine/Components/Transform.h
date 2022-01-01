#pragma once

#include "../Scene/Component.h"
#include "../Util3D.h"

namespace Game {
	class Transform : public Component {
	public:
		Vector3 position = Vector3::zero;
		Quaternion rotation = Quaternion::identity;
		Vector3 scale = Vector3::one;
		Matrix4x4 GetTransformationMatrix() { return Matrix4x4::TRS(position, rotation, scale); }
	};
}
