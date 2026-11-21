/*
 * ChunkStreamTest — streaming smoke test for a voxel-style chunk world.
 *
 * Runs 120 frames and, every single frame:
 *
 *   1. creates 32 scene objects from a MonoScript::onUpdate (the deadlock /
 *      vector-mutation case: Scene::createEnabled + addComponent during the
 *      update pass), each with a Transform and a ModelRender bound to one
 *      shared material;
 *   2. destroys the 32 objects that reached their lifetime (also from
 *      onUpdate), so the mesh RegisterCollection is churned every frame;
 *   3. replaces the geometry of a persistent "streamer" submesh with a mesh
 *      whose index count cycles 6 -> 12 -> 18 -> 24 through
 *      ModelRender::resetSubMesh(), i.e. the recorded DrawIndexed command has
 *      to be re-recorded every frame;
 *   4. partially uploads a constant-size mesh through
 *      RenderingMesh::getVertexBuffer()/getIndexBuffer() (Path A).
 *
 * It also sets the camera to 90 deg / near 0.01 / far 1000 and checks the
 * projection parameters are picked up.
 *
 * The binary exits with 0 after the last frame (it closes its own window with
 * a WINDOWEVENT_CLOSE, exactly like a window manager would) and with 1 if any
 * invariant broke. Run under the validation layers (Debug build) so that a
 * stale draw command/buffer misuse shows up as an [ERROR] line.
 */

#include <SDL.h>

#include "Rice/Engine.hpp"
#include "Rice/Engine/DefaultCoreLoader.hpp"
#include "Rice/Tests/Scenes/ChunkStreamTest.hpp"

int main() {
    using namespace Rice;
    using namespace Rice::Tests;
    std::printf("ChunkStreamTest: starting\n");
    ClientCore::runNew(new_ptr<DefaultCoreLoader>("ChunkStreamTest", ChunkStreamScene::create()));
    if (ChunkStreamStats::failures != 0) {
        std::printf("ChunkStreamTest: %u failure(s)\n", ChunkStreamStats::failures);
        return 1;
    }
    std::printf("ChunkStreamTest: done, %u frames, %u created, %u destroyed, %u resetSubMesh, "
                "%u partial uploads\n",
                ChunkStreamStats::frames, ChunkStreamStats::created,
                ChunkStreamStats::destroyed, ChunkStreamStats::resetSubMeshCalls,
                ChunkStreamStats::partialUploads);
    return 0;
}
