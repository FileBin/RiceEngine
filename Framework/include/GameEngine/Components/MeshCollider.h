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

				dbl sD(Vector3 p) const;
			};

			std::vector<Triangle> triangles;

			void build(Mesh& m);

			PhysMesh(Mesh& m);
		} *physMesh;
	public:
		MeshCollider() : Collider() {};
		void SetMesh(Mesh* m);
		void SetModel(Model* m);
		void SetModel(Model* m, int ignoredMat);
		dbl sdFunc(Vector3 p);

	};
}
