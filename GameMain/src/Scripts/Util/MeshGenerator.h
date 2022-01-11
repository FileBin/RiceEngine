#pragma once
#include <GameEngine\Vectors\Vector3.h>
#include <GameEngine\Vectors\Vector3i.h>
#include <GameEngine\Model.h>
#include <Scripts\Util\Tables.h>
#include <GameEngine\Util\SmartPointer.h>

class MeshGenerator {
private:
    static dbl GetPoint(dbl d1, dbl d2) {
        return 1 + d2 / (d1 - d2);
    }

    static Vector3 edgeToVert(int e, dbl d[], Vector3 verts[]) {
        const int* edge = Tables::edges[e];
        auto d1 = d[edge[0]];
        auto d2 = d[edge[1]];

        auto x = GetPoint(d1, d2);

        if (x < 0.01)
            x = 0;
        else if (x > 0.99)
            x = 1;

        Vector3 p = Vector3::Lerp(verts[edge[0]], verts[edge[1]], x);
        return p;
    }
public:
	static SmartPtr<Game::Mesh> GenerateMesh(std::function<dbl(Vector3)> sdFunc, Vector3i res = { 50, 50, 50 }, Vector3 _min = -Vector3::one, Vector3 _max = Vector3::one) {
        using namespace std;
        using byte = ::byte;
        auto size = _max - _min;

        auto mesh = new Game::Mesh();

        Vector3 step = size;

        step.x /= res.x;
        step.y /= res.y;
        step.z /= res.z;

        Vector3 verts[8];

        verts[0] = Tables::cubeVertices[0];
        verts[1] = Tables::cubeVertices[1];
        verts[2] = Tables::cubeVertices[2];
        verts[3] = Tables::cubeVertices[3];
        verts[4] = Tables::cubeVertices[4];
        verts[5] = Tables::cubeVertices[5];
        verts[6] = Tables::cubeVertices[6];
        verts[7] = Tables::cubeVertices[7];

        for (byte i = 0; i < 8; i++) {
            verts[i].x *= step.x;
            verts[i].y *= step.y;
            verts[i].z *= step.z;
        }

        function<void(num, num, num)> func = [&](num a, num b, num c) {
            Vector3 pos;
            pos.x = a * step.x;
            pos.y = b * step.y;
            pos.z = c * step.z;

            pos += _min;

            double d[8];
            byte _case = 0;
            byte pow = 1;

            for (byte i = 0; i < 8; i++) {
                auto lpos = pos + verts[i];

                auto depth = sdFunc(lpos);
                d[i] = depth;
                if (depth > 0)
                    _case += pow;
                pow <<= 1; // *= 2
            }

            auto m = new Game::Mesh();
            const int* edges = Tables::triangualtionTable[_case];
            // m.vertexBuffer.reserve(16);
            // m.indexBuffer.reserve(16);
            for (byte i = 0; i < 16; i++) {
                if (edges[i] == -1) break;
                auto vert = edgeToVert(edges[i], d, verts);
                vert += pos;
                m->vertexBuffer.push_back({ vert, {} });
                m->indexBuffer.push_back(i);
            }
            mesh->Combine(*m);
            delete m;
        };
        for (num i = 0; i < res.x; i++) {
            for (num j = 0; j < res.y; j++) {
                for (num k = 0; k < res.z; k++) {
                    func(i, j, k);
                }
            }
        }
        mesh->RecalculateNormals();
        mesh->ReclaculateBounds();

        return mesh;
	}
};
