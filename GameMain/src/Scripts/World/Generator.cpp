#include "pch.h"

#include "Chunk.h"
#include "World.h"
#include <Scripts\Util\Tables.h>

using std::function, std::vector;

float GetPoint(float d1, float d2) {
    return 1 + d2 / (d1 - d2);
}

Model* Chunk::GenerateModel() {
    auto mod = new Model();

    auto nMeshes = Voxel::GetMaterialCount();

    auto meshes = std::vector<std::shared_ptr<Mesh>>(nMeshes);

    for (auto& m : meshes) {
        m = std::make_shared<Mesh>();
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

Model* Chunk::GenerateSmoothModel(size_t step) {
    auto mod = new Model();
    size_t s = Chunk::ChunkSize;
    size_t loop = s * s * s;

    auto nMeshes = Voxel::GetMaterialCount();

    size_t matIdx = 0;

    auto meshes = std::vector<std::shared_ptr<Mesh>>(nMeshes);

    for (auto& m : meshes) {
        m = std::make_shared<Mesh>();
        m->layout = Vertex::GetLayout();
    }

    bool transp = false;

    Vector3 scale{};
    scale.x = scale.y = scale.z = step;

    function<void(num, num, num)> func = [&](num a, num b, num c) {
        Vector3i inChunkPos;
        inChunkPos.x = a - 1;
        inChunkPos.y = b - 1;
        inChunkPos.z = c - 1;

        Vector3i offset = position * s;
        Vector3i worldPos = inChunkPos + offset;

        float d[8];
        //auto norms = new Vector3[8];
        byte _case = 0;
        byte pow = 1;

        for (byte i = 0; i < 8; i++) {
            VoxelData vox;
            auto pos = worldPos + Tables::cubeVertices[i] * step;
            vox = world->GetVoxelData(pos);
            float depth = vox.depth;
            if (transp) {
                if(Voxel::IsTransparent(vox.index)) {
                    depth = world->GetTransparentVoxelDepth(pos, vox.index);
                } else {
                    depth = abs(depth);
                }
            }
            d[i] = depth;
            if (!Voxel::IsVoid(vox.index)) {
                if (transp) {
                    if (Voxel::IsTransparent(vox.index)) {
                        matIdx = vox.index;
                    }
                } else {
                    if (!Voxel::IsTransparent(vox.index)) {
                        matIdx = vox.index;
                    }
                }
            }
            if (depth >= 0) {
                _case += pow;
            }
            pow <<= 1; // *= 2
        }

        if (_case == 0x0 || _case == 0xff) return;

        function<Vector3(int)> edgeToVert = [&](int e) {
            const int* edge = Tables::edges[e];
            auto d1 = d[edge[0]];
            auto d2 = d[edge[1]];

            auto x = GetPoint(d1, d2);

            Vector3 p = Vector3::Lerp(Tables::cubeVertices[edge[0]] * step, Tables::cubeVertices[edge[1]] * step, x);
            return p;
        };

        vector<int> edges{};

        for (byte i = 0; i < 16; i++) {
            auto vert = Tables::triangualtionTable[_case][i];
            if (vert == -1)
                break;
            edges.push_back(vert);
        }
        Mesh m{};
        auto n = edges.size();
        if (n == 0) return;
        m.vertexBuffer.resize(n);
        m.indexBuffer.resize(n);
        for (byte i = 0; i < n; i++) {
            auto vert = edgeToVert(edges[i]);
            //var norm = edgeToNorm(edges[i]);
            vert += inChunkPos;
            m.vertexBuffer[i].position = vert;
            m.indexBuffer[i] = i;
            /*if (ChunkLoader.Smooth)
                m.Normals.Add(norm.normalized);*/
        }
        meshes[matIdx]->Combine(m);
    };

    auto a = ChunkSize;
    for (int i = 0; i < a; i += step) {
        for (int j = 0; j < a; j += step) {
            for (int k = 0; k < a; k += step) {
                func(i, j, k);
            }
        }
    }

    transp = true;

    for (int i = 0; i < a; i += step) {
        for (int j = 0; j < a; j += step) {
            for (int k = 0; k < a; k += step) {
                func(i, j, k);
            }
        }
    }

    mod->SetSubMeshesCount(nMeshes);
    for (size_t i = 0; i < nMeshes; i++) {
        auto mesh = meshes[i];
        mesh->RecalculateNormals();
        mesh->ReclaculateBounds();
        mod->SetSubMesh(mesh, i);
    }

    return mod;
}