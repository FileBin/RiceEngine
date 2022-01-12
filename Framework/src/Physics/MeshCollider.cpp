#include "pch.h"
#include <GameEngine\Components\MeshCollider.h>
#include <GameEngine\Util\DistanceEstimator.h>

namespace Game {
	void Game::MeshCollider::SetMesh(Mesh m) { mesh = m; }

	struct SimTriangle {
	public:
		Vector3 norm;
		Vector3 pos[3];
		dbl dist = 0;

		SimTriangle() = default;
		SimTriangle(Vector3 p1, Vector3 p2, Vector3 p3) {
			norm = Vector3::Cross(p1 - p2, p1 - p3);
			norm.Normalize();
			pos[0] = p1;
			pos[1] = p2;
			pos[2] = p3;
		}

		dbl sD(Vector3 p) const {
			auto d1 = Vector3::Dot(p - pos[0],Vector3::Cross(pos[1] - pos[0], norm));
			auto d2 = Vector3::Dot(p - pos[1],Vector3::Cross(pos[2] - pos[1], norm));
			auto d3 = Vector3::Dot(p - pos[2], Vector3::Cross(pos[0] - pos[2], norm));
			if (d1 < 0 && d2 < 0 && d3 < 0) {
				return abs(Util::sdPlane(p - pos[0], norm));
			}
			d1 = Util::sdLine(p, pos[0], pos[1]);
			d2 = Util::sdLine(p, pos[1], pos[2]);
			d3 = Util::sdLine(p, pos[2], pos[0]);
			return Math::Min(
				d1,
				Math::Min(
					d2,
					d3
				));

		}
	};

	dbl MeshCollider::sdFunc(Vector3 p) {
		using namespace std;
		p -= position;
		vector<SimTriangle> simTriangles{};
		auto n = mesh.indexBuffer.size();
		simTriangles.resize(n);
		size_t nTris = 0;

		for (size_t i = 0; i < n; i += 3) { // filtering

			auto iA = mesh.indexBuffer[i];
			auto iB = mesh.indexBuffer[i + 1];
			auto iC = mesh.indexBuffer[i + 2];

			const auto& vA = mesh.vertexBuffer[iA];
			const auto& vB = mesh.vertexBuffer[iB];
			const auto& vC = mesh.vertexBuffer[iC];

			auto& tri = simTriangles[nTris];

			tri = SimTriangle(vA.position, vB.position, vC.position);

			tri.dist = (p - (Vector3)vA.position).Length();

			for (auto i = nTris; i > 0; i--) { // sorting
				if (simTriangles[i - 1].dist > simTriangles[i].dist) {
					std::swap(simTriangles[i], simTriangles[i - 1]);
				} else break;
			}
			nTris++;
		}

		auto res_d = simTriangles[0].sD(p);

		nTris = min(nTris, 0xff);

		for (size_t i = 1; i < nTris; i++) {
			using namespace Util;
			const auto& plane = simTriangles[i];
			auto d = plane.sD(p);
			res_d = opJoin(d, res_d);
		}

		return res_d;
	}

	dbl MeshCollider::PhysMesh::Triangle::sD(Vector3 p) const {
		auto d1 = Vector3::Dot(p - pos[0], Vector3::Cross(pos[1] - pos[0], norm));
		auto d2 = Vector3::Dot(p - pos[1], Vector3::Cross(pos[2] - pos[1], norm));
		auto d3 = Vector3::Dot(p - pos[2], Vector3::Cross(pos[0] - pos[2], norm));
		if (d1 < 0 && d2 < 0 && d3 < 0) {
			return abs(Util::sdPlane(p - pos[0], norm));
		}
		d1 = Util::sdLine(p, pos[0], pos[1]);
		d2 = Util::sdLine(p, pos[1], pos[2]);
		d3 = Util::sdLine(p, pos[2], pos[0]);
		return Math::Min(
			d1,
			Math::Min(
				d2,
				d3
			));
	}
}