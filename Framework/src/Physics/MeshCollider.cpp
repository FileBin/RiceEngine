#include "pch.h"
#include <GameEngine\Components\MeshCollider.h>
#include <GameEngine\Util\DistanceEstimator.h>
#include <ppltasks.h>

namespace Game {
	void MeshCollider::SetMesh(Mesh* m) {
		_DELETE(physMesh);
		physMesh = new PhysMesh(*m);
	}

	void MeshCollider::SetModel(Model* m, int ignoredMat) {
		auto n = m->GetSubMeshesCount();
		auto combined = new Mesh();
		for (auto i = 0; i < n; i++) {
			if (i == ignoredMat) continue;
			combined->Combine(*m->GetSubMesh(i));
		}
		combined->ReclaculateBounds();
		auto pm = new PhysMesh(*combined);
		if (enabled) {
			std::lock_guard lock(engine->GetUpdateMutex());
			_DELETE(physMesh);
			physMesh = pm;
		} else {
			_DELETE(physMesh);
			physMesh = pm;
		}
		delete combined;
	}

	dbl MeshCollider::sdFunc(Vector3 p) {
		using namespace std;
		p -= position;
		if (physMesh == nullptr || !physMesh->bounds.IsInBounds(p)) return DBL_MAX;
		auto n = physMesh->triangles.size();
		dbl dist = DBL_MAX;
		std::vector<PhysMesh::Triangle*> initTris(1);
		for (size_t i = 0; i < n; i++) { // search
			auto& tri = physMesh->triangles[i];
			auto d = p - tri.pos[0];
			auto l = d.SqrLength();
			tri.checked = false;
			if (dist >= l) {
				dist = l;
				initTris[0] = &tri;
			}
		}

		dbl res_d = DBL_MAX;

		std::vector<PhysMesh::Triangle*> buf;

		buf.reserve(0xff);
		initTris.reserve(0xff);

		for (size_t i = 0; i < 2; i++) {
			for (auto tri : initTris) {
				if (tri == nullptr) return DBL_MAX;
				if (tri->checked) continue;
				tri->checked = true;
				buf.insert(buf.end(), tri->tris.begin(), tri->tris.end());
				res_d = Util::opJoin(res_d, tri->sD(p));
			}
			initTris = buf;
		}

		return res_d;
	}

	dbl MeshCollider::PhysMesh::Triangle::sD(Vector3 p) const {
		auto d1 = Vector3::Dot(p - pos[0], Vector3::Cross(pos[1] - pos[0], norm));
		auto d2 = Vector3::Dot(p - pos[1], Vector3::Cross(pos[2] - pos[1], norm));
		auto d3 = Vector3::Dot(p - pos[2], Vector3::Cross(pos[0] - pos[2], norm));
		if (d1 < 0 && d2 < 0 && d3 < 0 || d1 > 0 && d2 > 0 && d3 > 0) {
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

#define FAST_GENERATE 1

	void MeshCollider::PhysMesh::build(Mesh& m) {
		//Log::log(Log::Debug, L"GenerationPhysMesh...");
		//auto clock = std::chrono::steady_clock::now();
		bounds.Min = (Vector3)m.bounds.Min - Vector3::one * PHYS_FIXED_STEP;
		bounds.Max = (Vector3)m.bounds.Max + Vector3::one * PHYS_FIXED_STEP;
		auto n = m.indexBuffer.size() / 3;
		triangles.resize(n);
		for (size_t i = 0; i < n; i++) {
			auto triIdx = i * 3;

			auto& pos0 = m.vertexBuffer[m.indexBuffer[triIdx]].position;

			triangles[i].pos[0] = pos0;
			triangles[i].pos[1] = m.vertexBuffer[m.indexBuffer[triIdx + 1]].position;
			triangles[i].pos[2] = m.vertexBuffer[m.indexBuffer[triIdx + 2]].position;
			triangles[i].calcNorm();
		}
		for (size_t i = 0; i < n; i++) {
			auto& first = triangles[i];
			for (auto j = i + 1; j < n; j++) {
				auto& second = triangles[j];
#ifdef FAST_GENERATE
				if ((first.pos[0] - second.pos[0]).SqrLength() < 2.5) {
					first.tris.push_back(&second);
					second.tris.push_back(&first);
				}
#else // FAST_GENERATE

				for (byte k = 0; k < 3; k++) {
					for (byte l = 0; l < 3; l++) {
						if ((first.pos[k] - second.pos[l]).SqrLength() < .1) {
							first.tris.push_back(&second);
							second.tris.push_back(&first);

						}
					}
				}
#endif
			}
		}
		//Log::log(Log::INFO, L"Generation took: {}ms", (std::chrono::steady_clock::now() - clock).count() * .001 * .001);
		//Log::log(Log::Debug, L"GenerationPhysMesh...end");
	}

	MeshCollider::PhysMesh::PhysMesh(Mesh& m) {
		build(m);
	}
}