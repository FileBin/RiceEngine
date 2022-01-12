#pragma once

#include "Collider.h"
#include "../Model.h"

namespace Game {
	class MeshCollider : public Collider {
	protected:
		class PhysMesh {
		public:
			struct Triangle {
			public:
				std::vector<Triangle*> tris;
				Vector3 norm;
				Vector3 pos[3];
				dbl dist = 0;

				Triangle() = default;
				Triangle(Vector3 p1, Vector3 p2, Vector3 p3) {
					norm = Vector3::Cross(p1 - p2, p1 - p3);
					norm.Normalize();
					pos[0] = p1;
					pos[1] = p2;
					pos[2] = p3;
				}

				dbl sD(Vector3 p) const;
			};

			std::vector<Triangle> triangles;
		} physMesh;
		Mesh mesh;
	public:
		MeshCollider() : Collider() {};
		void SetMesh(Mesh m);
		dbl sdFunc(Vector3 p);
	};
}
