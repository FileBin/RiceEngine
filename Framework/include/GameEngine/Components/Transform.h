#pragma once

#include "../Scene/Component.h"
#include "../Util3D.h"

namespace Game {
	class Transform : public Component {
	private:
		Vector3 position = Vector3::zero;
		Quaternion rotation = Quaternion::identity;
		Vector3 scale = Vector3::one;
	public:
		Vector3 GetPosition() {
			std::lock_guard l(mut);
			return position;
		}

		Quaternion GetRotation() {
			std::lock_guard l(mut);
			return rotation;
		}

		Vector3 GetScale() {
			std::lock_guard l(mut);
			return scale;
		}

		void SetPosition(Vector3 pos) {
			std::lock_guard l(mut);
			position = pos;
		}

		void SetRotation(Quaternion q) {
			std::lock_guard l(mut);
			rotation = q;
		}

		void SetScale(Vector3 s) {
			std::lock_guard l(mut);
			scale = s;
		}

		void SetPRS(Vector3 pos = Vector3::zero, Quaternion rot = Quaternion::identity, Vector3 s = Vector3::one){
			std::lock_guard l(mut);
			position = pos;
			rotation = rot;
			scale = s;

		}
		std::mutex mut;
		Matrix4x4 GetTransformationMatrix() { 
			std::lock_guard l(mut);
			return Matrix4x4::TRS(position, rotation, scale); 
		}
	};
}
