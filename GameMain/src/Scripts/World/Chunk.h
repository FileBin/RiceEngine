#pragma once
#include <vector>
#include <GameEngine\Model.h>
#include <functional>

#include "Voxel.h"

using namespace Game;

struct Chunk {
	std::vector<Voxel> voxels = {};
	static const int ChunkSize;
    Vector3i position{};

	Chunk() {
		voxels.resize((INT64)ChunkSize * ChunkSize * ChunkSize);
	}

    Voxel& GetVoxel(int x, int y, int z) {
        return voxels[((INT64)x * ChunkSize + y) * ChunkSize + z];
    }

    void SetVoxel(Voxel vox) {
        auto x = vox.position.x;
        auto y = vox.position.y;
        auto z = vox.position.z;
        voxels[(x * ChunkSize + y) * ChunkSize + z] = vox;
    }

    Voxel& GetVoxel(Vector3i pos) {
        return GetVoxel(pos.x, pos.y, pos.z);
    }

	Model* GenerateModel(){
        Model* mod = new Model();

        auto mesh = new Mesh();

        mesh->layout = Vertex::GetLayout();

        std::function<void(int, int, int)> func = [&](int x, int y, int z) {
            Voxel b1, b2;
            Vector3i inChunkPos{ x,y,z };
            //Vector3i offset = position * ChunkSize;
            //var worldPos = inChunkPos + offset;

            if (x < 0 || y < 0 || z < 0) {
                b1.position = inChunkPos;
                b1.isVoid = true;//b1 = world.GetVoxel(worldPos);
            } else {
                b1 = GetVoxel(inChunkPos);
            }

            for (int i = 0; i < 3; i++) {
                Vector3i pos = inChunkPos;
                pos[i] += 1;

                if (pos[i] >= ChunkSize || (pos.x < 0 || pos.y < 0 || pos.z < 0)) {
                    //var wp = pos + offset;
                    //b2 = world.GetVoxel(wp);
                    b2.position = pos;
                    b2.isVoid = true;
                } else {
                    b2 = GetVoxel(pos);
                }
                Voxel *empty, *full;

                auto o = Vector3::one * .5f;

                auto angle = -90.;

                if (b1.isVoid) {
                    empty = &b1;
                    full = &b2;
                    o[i] = 1.5f;
                    if (b2.isVoid) {
                        continue;
                    }
                } else if (b2.isVoid) {
                    angle = 90;
                    empty = &b2;
                    full = &b1;
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
                mesh->Combine(c);
            }
        };

        for (int i = -1; i < ChunkSize; i++) {
            for (int j = -1; j < ChunkSize; j++) {
                for (int k = -1; k < ChunkSize; k++) {
                    func(i, j, k);
                }
            }
        }

        mod->SetSubMeshesCount(1);
        mod->SetSubMesh(*mesh, 0);
        return mod;
	}
};
