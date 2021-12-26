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

    auto meshes = std::vector<Mesh*>(nMeshes);

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

            auto o = Vector3::one * .5f;

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

Model* Chunk::GenerateSmoothModel() {
    auto mod = new Model();
    size_t s = Chunk::ChunkSize;
    size_t loop = s * s * s;

    auto nMeshes = Voxel::GetMaterialCount();

    size_t matIdx = 0;

    auto meshes = std::vector<Mesh*>(nMeshes);

    for (auto& m : meshes) {
        m = new Mesh();
        m->layout = Vertex::GetLayout();
    }

    function<void(num, num, num)> func = [&](num a, num b, num c) {
        Vector3i inChunkPos;
        inChunkPos.x = a - 1;
        inChunkPos.y = b - 1;
        inChunkPos.z = c - 1;

        Vector3i offset = position * s;
        Vector3i worldPos = inChunkPos + offset;

        auto d = new float[8];
        //auto norms = new Vector3[8];
        int _case = 0;
        int pow = 1;

        for (byte i = 0; i < 8; i++) {
            auto vox = world->GetVoxelData(worldPos + Tables::cubeVertices[i]);
            float depth = vox.depth;
            d[i] = depth;
            if (!Voxel::IsVoid(vox.index))
                matIdx = vox.index;
            //norms[i] = vox.normal;
            if (depth > 0) {
                _case += pow;
            }
            pow *= 2;
        }

        function<Vector3(int)> edgeToVert = [&](int e) {
            const int* edge = Tables::edges[e];
            auto d1 = d[edge[0]];
            auto d2 = d[edge[1]];

            auto x = GetPoint(d1, d2);

            Vector3 p = Vector3::Lerp(Tables::cubeVertices[edge[0]], Tables::cubeVertices[edge[1]], x);
            return p;
        };

        /* function<Vector3(int)> edgeToNorm = [&](int e) {
             const int* edge = Tables::edges[e];
             auto d1 = d[edge[0]];
             auto d2 = d[edge[1]];

             var x = GetPoint(d1, d2);

             var p = Vector3.Lerp(norms[edge.x], norms[edge.y], x);
             return p;
         };*/

        vector<int> edges{};

        for (byte i = 0; i < 16; i++) {
            auto vert = Tables::triangualtionTable[_case][i];
            if (vert == -1)
                break;
            edges.push_back(vert);
        }
        Mesh m{};
        auto n = edges.size();
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