#pragma once

#include "Collider.h"
#include "../Model.h"

namespace Game {
	class MeshCollider : public Collider {
	protected:
		class PhysMesh {
		public:
			struct Bounds {
			public:
				Vector3 Min;
				Vector3 Max;
				bool IsInBounds(Vector3 p) {
					if (p.x > Max.x || p.x < Min.x || p.y < Min.y || p.y > Max.y || p.z > Max.z || p.z < Min.z) {
						return false;
					}
					return true;
				}
			} bounds;
			struct Triangle {
			public:
				std::vector<Triangle*> tris;
				Vector3 norm;
				Vector3 pos[3];
				Vector3 center;
				bool checked = false;

				Triangle() = default;
				Triangle(Vector3 p1, Vector3 p2, Vector3 p3) {
					calcNorm();
					pos[0] = p1;
					pos[1] = p2;
					pos[2] = p3;
				}

				void calcNorm() {
					norm = Vector3::Cross(pos[0] - pos[1], pos[0] - pos[2]);
					norm.Normalize();
				}

				void calcCenter() {
					center = pos[0] + pos[1] + pos[2];
					center /= 3;
				}

				dbl sD(Vector3 p) const;
			};

			std::vector<Triangle> triangles;

			void build(Mesh& m);

			Triangle& getNearestTriangle(Vector3 pos);

			PhysMesh(Mesh& m);
		};
		PhysMesh *physMesh = nullptr;
	public:
		MeshCollider() : Collider() {};
		void SetMesh(Mesh* m);
		void SetModel(Model* m);
		void SetModel(Model* m, int ignoredMat);
		dbl sdFunc(Vector3 p);

		Vector3 GetNormal(Vector3 pos, dbl eps);

	};
}