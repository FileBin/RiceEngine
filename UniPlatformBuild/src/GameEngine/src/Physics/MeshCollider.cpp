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

	MeshCollider::PhysMesh::Triangle& MeshCollider::PhysMesh::getNearestTriangle(Vector3 p) {
		MeshCollider::PhysMesh::Triangle* triangle = nullptr;
		dbl dist = DBL_MAX;
		auto n = triangles.size();
		for (size_t i = 1; i < n; i++) { // search
			auto& tri = triangles[i];
			auto d = p - tri.center;
			auto l = d.SqrLength();
			tri.checked = false;
			if (dist >= l) {
				dist = l;
				triangle = &tri;
			}
		}
		return *triangle;
	}

	dbl MeshCollider::sdFunc(Vector3 p) {
		using namespace std;
		p = p * transform->GetInvTransformationMatrix();
		if (physMesh == nullptr || !physMesh->bounds.IsInBounds(p)) return std::numeric_limits<dbl>().quiet_NaN();
		auto& initTri = physMesh->getNearestTriangle(p);
		if (&initTri == nullptr) return std::numeric_limits<dbl>().quiet_NaN();
		std::vector<PhysMesh::Triangle*> initTris(physMesh->getNearestTriangle(p).tris);

		dbl res_d = initTri.sD(p);

		std::vector<PhysMesh::Triangle*> buf;

		buf.reserve(0x2000);

		for (size_t i = 0; i < 2; i++) {
			for (auto tri : initTris) {
				if (tri == nullptr) continue;
				if (tri->checked) continue;
				tri->checked = true;
#ifndef _DEBUG
				buf.insert(buf.end(), tri->tris.begin(), tri->tris.end());
#endif
				auto sD =tri->sD(p);
				res_d = Util::opJoin(res_d, sD > 0 ? sD : DBL_MAX);
			}
#ifndef _DEBUG
			initTris = buf;
			if (initTris.empty())
				break;
			buf.clear();
#else
			break;
#endif
		}
		return res_d;
	}

	Vector3 MeshCollider::GetNormal(Vector3 pos, dbl eps) {
		//auto norm0 = physMesh->getNearestTriangle(pos - position).norm;
		//return norm0;
		return Math::GetNorm([this](Vector3 p) { return sdFunc(p); }, pos, eps);
	}

	dbl MeshCollider::PhysMesh::Triangle::sD(Vector3 p) const {
		auto d1 = Vector3::Dot(p - pos[0], Vector3::Cross(pos[1] - pos[0], norm));
		auto d2 = Vector3::Dot(p - pos[1], Vector3::Cross(pos[2] - pos[1], norm));
		auto d3 = Vector3::Dot(p - pos[2], Vector3::Cross(pos[0] - pos[2], norm));
		if (d1 < 0 && d2 < 0 && d3 < 0 || d1 > 0 && d2 > 0 && d3 > 0) {
			return Util::sdPlane(p - pos[0], norm);
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

#define FAST_GENERATE

	void MeshCollider::PhysMesh::build(Mesh& m) {
		//Log::log(Log::Debug, L"GenerationPhysMesh...");
		//auto clock = std::chrono::steady_clock::now();
		bounds.Min = (Vector3)m.bounds.Min - Vector3::one * PHYS_FIXED_STEP;
		bounds.Max = (Vector3)m.bounds.Max + Vector3::one * PHYS_FIXED_STEP;
		auto n = m.indexBuffer.size() / 3;
		triangles.resize(n);
		for (size_t i = 0; i < n; i++) {
			auto triIdx = i * 3;

			auto& triangle = triangles[i];

			triangle.pos[0] = m.vertexBuffer[m.indexBuffer[triIdx]].position;
			triangle.pos[1] = m.vertexBuffer[m.indexBuffer[triIdx + 1]].position;
			triangle.pos[2] = m.vertexBuffer[m.indexBuffer[triIdx + 2]].position;
			triangle.calcNorm();
			triangle.calcCenter();
		}
		for (size_t i = 0; i < n; i++) {
			auto& first = triangles[i];
			for (auto j = i + 1; j < n; j++) {
				auto& second = triangles[j];
#ifdef FAST_GENERATE
				if ((first.center - second.center).SqrLength() <= 4.) {
					first.tris.push_back(&second);
					second.tris.push_back(&first);
				}
#else // FAST_GENERATE

				for (byte k = 0; k < 3; k++) {
					for (byte l = 0; l < 3; l++) {
						if ((first.pos[k] - second.pos[l]).SqrLength() < .1) {
							first.tris.push_back(&second);
							second.tris.push_back(&first);
							break;
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