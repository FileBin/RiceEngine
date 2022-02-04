#include "pch.h"

#include "Chunk.h"
#include "World.h"
#include <Scripts\Util\Tables.h>
#include "Scripts\Util\Util.h"
#include <GameEngine\Math.h>

using std::function, std::vector;

float GetPoint(float d1, float d2) {
    return 1 + d2 / (d1 - d2);
}

void Chunk::UpdateModel(SceneRender& ren, size_t lod) {
    auto idx = 1 << lod;
    auto& _model = model[lod];
    bool alreadyLoaded = !_model.IsNull();
    auto otherM = GenerateSmoothModel(idx);
#ifdef MULTI_RENDER
    uuid.GetRender()->SetModel(uuid, otherM, Matrix4x4::Translation(World::TransformToWorldPos(position)));
#else
    render->SetModel(otherM);
#endif
    _model.Release();
    collider->SetModel(otherM, VoxelTypeIndex::V_WATER);
}

Model* Chunk::GenerateModel() {
    auto mod = new Model();

    auto nMeshes = Voxel::GetMaterialCount();

    auto meshes = std::vector<SmartPtr<Mesh>>(nMeshes);

    for (auto& m : meshes) {
        m = new Mesh();
        m->layout = Vertex::GetLayout();
    }

    function<void(int, int, int)> func = [&](int x, int y, int z) {
        bool v1, v2;
        size_t idx1 = UINT32_MAX, idx2 = UINT32_MAX;
        Vector3i inChunkPos{ x,y,z };
        Vector3i offset = position * ChunkSize;
        Vector3i worldPos = inChunkPos + offset;

        if (x < 0 || y < 0 || z < 0) {
            idx1 = world->GetVoxelData(worldPos).index;
        } else {
            idx1 = GetData(inChunkPos).index;
        }

        v1 = Voxel::IsVoid(idx1);

        for (int i = 0; i < 3; i++) {
            Vector3i pos = inChunkPos;
            pos[i] += 1;

            if (pos[i] >= ChunkSize) {
                Vector3i wp = pos + offset;
                idx2 = world->GetVoxelData(wp).index;
            } else {
                idx2 = GetData(pos).index;
            }

            v2 = Voxel::IsVoid(idx2);

            auto o = Vector3::one * .5;

            auto angle = -90.;

            size_t matIdx = UINT32_MAX;

            if (v1) {
                o[i] = 1.5f;
                if (v2) {
                    continue;
                } else {
                    matIdx = idx2;
                }
            } else if (v2) {
                matIdx = idx1;
                angle = 90;
            } else {
                continue;
            }
            auto c = Mesh::quad;
            c.Translate({ 0,0, .5 });
            Vector3 p = inChunkPos;
            p += o;
            //auto q = Quaternion::LookAt((Vector3)full->position, (Vector3)empty->position);
            auto q = Quaternion::identity;
            if (i == 0) {
                q = Quaternion::FromAxisAngle(Vector3::up, angle);
            } else if (i == 1) {
                q = Quaternion::FromAxisAngle(Vector3::left, angle);
            } else if (angle < 0) {
                q = Quaternion::FromAxisAngle(Vector3::left, 180);
            }
            c.Rotate(q);
            c.Translate(p);
            meshes[matIdx]->Combine(c);
        }
    };
    auto a = ChunkSize;
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < a; j++) {
            for (int k = 0; k < a; k++) {
                func(i, j, k);
            }
        }
    }

    mod->SetSubMeshesCount(nMeshes);
    for (size_t i = 0; i < nMeshes; i++) {

        mod->SetSubMesh(meshes[i], i);
    }
    return mod;
}

struct Piece {
    float d[8];
    num matIdx = -1;
    byte Case = 0;

    Mesh* CreateMesh(Vector3 pos, int step, bool doubleSided=false) {
        if (Case == 0 || Case == 255) return new Mesh();
        function<Vector3(int)> edgeToVert = [&](int e) {
            const int* edge = Tables::edges[e];
            auto d1 = d[edge[0]];
            auto d2 = d[edge[1]];

            auto x = GetPoint(d1, d2);

            if (x < 0.01)
                x = 0;
            else if (x > 0.99)
                x = 1;

            Vector3 p = Vector3::Lerp(Tables::cubeVertices[edge[0]] * step, Tables::cubeVertices[edge[1]] * step, x);
            return p;
        };
        Mesh* m = new Mesh();
        const int* edges = Tables::triangualtionTable[Case];
       // m.vertexBuffer.reserve(16);
       // m.indexBuffer.reserve(16);
        for (byte i = 0; i < 16; i++) {
            if (edges[i] == -1) break;
            auto vert = edgeToVert(edges[i]);
            vert += pos;
            m->vertexBuffer.push_back({ vert, {} });
            m->indexBuffer.push_back(i);
        }
        if (doubleSided) {
            for (int i = m->indexBuffer.size()-1; i >= 0; i--) {
                m->indexBuffer.push_back(m->indexBuffer[i]);
            }
        }
        return m;
    }
};

Model* Chunk::GenerateSmoothModel(size_t step) {
    auto mod = new Model();
    size_t s = Chunk::ChunkSize;
    size_t loop = s * s * s;

    auto nMeshes = Voxel::GetMaterialCount();

    auto meshes = std::vector<SmartPtr<Mesh>>(nMeshes);

    for (auto& m : meshes) {
        m = new Mesh();
        m->layout = Vertex::GetLayout();
    }

    Vector3 scale{};
    scale.x = scale.y = scale.z = step;

    function<void(num, num, num)> func = [&](num a, num b, num c) {
        Vector3i inChunkPos;
        inChunkPos.x = a - 1;
        inChunkPos.y = b - 1;
        inChunkPos.z = c - 1;

        Vector3i offset = position * s;
        Vector3i worldPos = inChunkPos + offset;

        Piece solid{};
        Piece transparent[8];
        byte pow = 1;

        for (byte i = 0; i < 8; i++) {
            VoxelData vox;
            auto lpos = inChunkPos + Tables::cubeVertices[i] * step;
            auto wpos = worldPos + Tables::cubeVertices[i] * step;
            if (!((lpos.x + 1) & (lpos.y + 1) & (lpos.z + 1))) {
                vox = world->GetVoxelData(wpos);
            } else {
                vox = GetData(lpos);
            }
            auto isT = Voxel::IsTransparent(vox.index);
            auto isV = Voxel::IsVoid(vox.index);

            float depth = vox.depth;
            solid.d[i] = depth;
            for (byte j = 0; j < 8; j++) {
                auto& d = transparent[j].d[i];
                if (isT) {
                    if (!isV) {
                        transparent[j].matIdx = vox.index;
                    }
                    d = world->GetTransparentVoxelDepth(wpos, vox.index);
                } else {
                    d = abs(depth);
                }
                if (d >= 0) {
                    transparent[j].Case += pow;
                }
            }
            if (!isV && !isT) {
                    solid.matIdx = vox.index;
            }
            if (depth >= 0) {
                solid.Case += pow;
            }
            pow <<= 1; // *= 2
        }

        if (solid.matIdx > 0) {
            auto& m = *solid.CreateMesh(inChunkPos, step);
            meshes[solid.matIdx]->Combine(m);
            delete& m;
        }
        for (byte i = 0; i < 8; i++) {
            if (transparent[i].matIdx == -1) continue;
            auto& m = *transparent[i].CreateMesh(inChunkPos, step, true);
            meshes[transparent[i].matIdx]->Combine(m);
            delete& m;
        }
    };

    auto a = ChunkSize;
    for (int i = 0; i < a; i += step) {
        for (int j = 0; j < a; j += step) {
            for (int k = 0; k < a; k += step) {
                func(i, j, k);
            }
        }
    }

   // transp = true;

    /*step *= 4;
    step = min(step, 8);

    for (int i = 0; i < a; i += step) {
        for (int j = 0; j < a; j += step) {
            for (int k = 0; k < a; k += step) {
                func(i, j, k);
            }
        }
    }*/

    mod->SetSubMeshesCount(nMeshes);
    for (size_t i = 0; i < nMeshes; i++) {
        auto mesh = meshes[i];
        mesh->RecalculateNormals();
        mesh->ReclaculateBounds();
        mod->SetSubMesh(mesh, i);
    }

    return mod;
}