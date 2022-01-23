#pragma once

#include "../Scene/Component.h"
#include "../Scene/SceneObject.h"
#include "../Util3D.h"

namespace Game {
	class Transform : public Component {
	private:
		std::shared_mutex mut;
		Transform* parent = nullptr;
		Vector3 position = Vector3::zero;
		Quaternion rotation = Quaternion::identity;
		Vector3 scale = Vector3::one;
	public:

		void OnEnable() {
			auto parentO = &GetSceneObject().GetParent();
			if (parentO != nullptr) {
				auto transforms = parentO->GetComponents<Transform>();
				if (!transforms.empty())
					parent = transforms.front();
			}

		}

		Vector3 GetPosition() {
			std::shared_lock l(mut);
			return position;
		}

		Quaternion GetRotation() {
			std::shared_lock l(mut);
			return rotation;
		}

		Vector3 GetScale() {
			std::shared_lock l(mut);
			return scale;
		}

		void SetPosition(Vector3 pos) {
			std::unique_lock l(mut);
			position = pos;
		}

		void SetRotation(Quaternion q) {
			std::unique_lock l(mut);
			rotation = q;
		}

		void SetScale(Vector3 s) {
			std::unique_lock l(mut);
			scale = s;
		}

		void SetPRS(Vector3 pos = Vector3::zero, Quaternion rot = Quaternion::identity, Vector3 s = Vector3::one){
			std::unique_lock l(mut);
			position = pos;
			rotation = rot;
			scale = s;

		}

		Matrix4x4 GetTransformationMatrix() { 
			std::shared_lock l(mut);
			if(parent != nullptr)
				return Matrix4x4::TRS(position, rotation, scale) * parent->GetTransformationMatrix();
			return Matrix4x4::TRS(position, rotation, scale); 
		}
	};
}
