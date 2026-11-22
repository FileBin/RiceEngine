/*
 * ComputeIndirectTest — milestone M3 of docs/tetrahedral-mesh.md on the engine
 * side: a compute pipeline, a storage buffer (SSBO), a pipeline barrier, an
 * indirect draw and a buffer readback, proved on a trivial shader instead of the
 * GPU mesher.
 *
 * One frame records, in a single primary command buffer:
 *
 *   1. a compute dispatch (shaders/m3_setup.comp.spv, one 64-wide workgroup)
 *      that writes four SSBOs: 64 marker words, six vertex positions, six
 *      indices, and one VkDrawIndexedIndirectCommand;
 *   2. a memory barrier compute-write -> vertex/index/indirect-read (§6.4) -
 *      without it the draw is free to fetch the buffers before the dispatch
 *      filled them; setting M3_NO_BARRIER=1 omits it so the negative run can be
 *      observed (lavapipe serialises, so it is expected to survive there, and
 *      the run says so);
 *   3. a render pass that clears to black and draws the compute-written
 *      geometry twice: once with drawIndexedIndirect() from the compute-written
 *      record, once with the equivalent direct drawIndexed(3, 3, 0). The
 *      fragment shader reads the compute-written marker word and only emits the
 *      orange colour when it is the expected value, so a black frame means the
 *      shader did not see the dispatch's output;
 *   4. captureColor(): the frame's colour image is copied into a host-visible
 *      buffer inside the same submission.
 *
 * After the frame fence (GraphicsManager::sync) every buffer is read back on the
 * CPU and asserted:
 *
 *   - 64/64 marker words equal i * 3 + 7                     (SSBO readback)
 *   - the six vertex positions and six indices are what compute wrote
 *   - the indirect draw covered pixels, and exactly as many as the equivalent
 *     direct draw of the same range                       (count/firstIndex ok)
 *   - a direct draw of the *other* index range (the decoy triangle at
 *     firstIndex 0) covers far fewer pixels, so firstIndex really was honoured
 *     rather than ignored
 *
 * It runs three frames (indirect, direct same range, direct first range), prints
 * one PASS/FAIL line per check and exits 0 only if all of them pass.
 */

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include "Rice/Engine/Log.hpp"
#include "Rice/Engine/Window.hpp"
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/GL/StorageBuffer.hpp"

using namespace Rice;
using namespace Rice::Graphics;

namespace {

constexpr uint kWords = 64;
constexpr uint kVertexCount = 6;
constexpr uint kIndexCount = 6;
constexpr uint kIndirectRecordBytes = 20; // sizeof(VkDrawIndexedIndirectCommand)

struct Check {
    uint passes = 0;
    uint failures = 0;

    void expect(bool ok, const char *what, const char *detail = "") {
        if (ok) {
            ++passes;
            std::printf("ComputeIndirectTest: PASS  %s %s\n", what, detail);
        } else {
            ++failures;
            std::printf("ComputeIndirectTest: FAIL  %s %s\n", what, detail);
        }
    }
};

/* The three draws the test compares. Same pipeline, same buffers, same frame
 * layout; only where the index count comes from differs. */
enum class DrawVariant {
    Indirect,        // drawIndexedIndirect(): everything from the SSBO
    DirectSameRange, // drawIndexed(3, firstIndex 3): the same triangle directly
    DirectFirstRange // drawIndexed(3, firstIndex 0): the decoy, must differ
};

uint countCoveredPixels(const vec<uint8_t> &rgba) {
    uint covered = 0;
    for (size_t i = 0; i + 3 < rgba.size(); i += 4)
        if (rgba[i] | rgba[i + 1] | rgba[i + 2]) // alpha is ignored
            ++covered;
    return covered;
}

struct Resources {
    ptr<Shader> compute;
    ptr<Shader> draw;
    ptr<StorageBuffer> marker;
    ptr<StorageBuffer> verts;
    ptr<StorageBuffer> indices;
    ptr<StorageBuffer> draws;
    ptr<StorageBuffer> pixels;

    Vector2i frame_size{0, 0};
    vec<uint8_t> pixels_cpu;
    bool barrier = true;

    void release() {
        pixels.reset();
        draws.reset();
        indices.reset();
        verts.reset();
        marker.reset();
        draw.reset();
        compute.reset();
    }
};

ptr<CommandBuffer> record_frame(ptr<GraphicsManager> g_mgr, Resources &r, DrawVariant variant) {
    auto cmd = new_ptr<CommandBuffer>(g_mgr);

    // --- compute: one workgroup of 64 invocations fills all four SSBOs -------
    // (shaders/m3_setup.comp declares local_size_x = 64, so one group covers the
    // 64 marker words.)
    cmd->bindStorageBuffer(r.marker, 0);
    cmd->bindStorageBuffer(r.verts, 1);
    cmd->bindStorageBuffer(r.indices, 2);
    cmd->bindStorageBuffer(r.draws, 3);
    cmd->dispatch(r.compute, 1, 1, 1);

    // --- the barrier the draw depends on (§6.4) -----------------------------
    if (r.barrier)
        cmd->memoryBarrier(BarrierStage::ComputeShader, BarrierAccess::ShaderWrite,
                           BarrierStage::VertexInput | BarrierStage::DrawIndirect |
                               BarrierStage::FragmentShader | BarrierStage::Transfer,
                           BarrierAccess::VertexAttributeRead | BarrierAccess::IndexRead |
                               BarrierAccess::IndirectCommandRead | BarrierAccess::ShaderRead |
                               BarrierAccess::TransferRead);

    // --- the draw -----------------------------------------------------------
    // Inline contents: the clear, the binds and the draw are recorded straight
    // into this primary command buffer (a compute dispatch cannot be inside a
    // render pass, so the pass starts after it).
    cmd->beginRenderPass({0, 0, 1, 1}, false);
    cmd->clearRenderTarget({0.f, 0.f, 0.f}, 1.f);

    cmd->setActiveShader(r.draw);
    cmd->bindVertexBuffer(r.verts);
    cmd->bindIndexBuffer(r.indices);
    // The fragment shader reads the compute-written marker: a draw that ran
    // before the dispatch colours nothing and the pixel count is 0.
    cmd->bindStorageBuffer(r.marker, 0);

    switch (variant) {
    case DrawVariant::Indirect:
        cmd->drawIndexedIndirect(r.draws, 0, 1);
        break;
    case DrawVariant::DirectSameRange:
        cmd->drawIndexed(3, 3, 0);
        break;
    case DrawVariant::DirectFirstRange:
        cmd->drawIndexed(3, 0, 0);
        break;
    }

    cmd->endRenderPass();

    // --- read the frame back (inside the same submission) -------------------
    cmd->captureColor(r.pixels);

    cmd->buildAll();
    return cmd;
}

/* Submits `cmd` until a frame really goes through (a swapchain rebuild during
 * startup makes GraphicsManager::executeCmd() skip one) and waits for it. */
bool submit_and_wait(ptr<GraphicsManager> g_mgr, ptr<Window> win, ptr<CommandBuffer> cmd) {
    for (uint attempt = 0; attempt < 10; ++attempt) {
        win->update();
        g_mgr->update();
        if (g_mgr->executeCmd(cmd)) {
            g_mgr->sync();
            return true;
        }
    }
    return false;
}

} // namespace

int main() {
    Log::init();

    Check check;

    const char *no_barrier = std::getenv("M3_NO_BARRIER");
    const bool want_barrier = no_barrier == nullptr;

    std::printf("ComputeIndirectTest: starting (barrier %s)\n",
                want_barrier ? "on" : "OFF (M3_NO_BARRIER)");

    auto win = Window::create({"ComputeIndirectTest", 480, 360});
    auto g_mgr = GraphicsManager::create();
    g_mgr->init(win);

    Resources r;
    r.barrier = want_barrier;

    // The compositor decides the window's real size (sway tiles it), and the
    // swapchain follows a frame later. Wait until the drawable size has stopped
    // moving before sizing the pixel buffer: its size is baked into the copy
    // region of the recorded frame.
    Vector2i last{0, 0};
    uint stable = 0;
    for (uint i = 0; i < 300 && stable < 10; ++i) {
        win->update();
        g_mgr->update();
        const Vector2i now = g_mgr->getFrameSize();
        stable = (now.x == last.x && now.y == last.y && now.x > 0) ? stable + 1 : 0;
        last = now;
        if (stable == 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    r.frame_size = g_mgr->getFrameSize();
    std::printf("ComputeIndirectTest: frame is %lldx%lld\n", (long long)r.frame_size.x,
                (long long)r.frame_size.y);

    const size_t pixel_bytes = (size_t)r.frame_size.x * r.frame_size.y * 4;
    r.pixels_cpu.resize(pixel_bytes);

    // --- resources ----------------------------------------------------------
    r.marker = new_ptr<StorageBuffer>(g_mgr, kWords * sizeof(uint));
    r.verts = new_ptr<StorageBuffer>(g_mgr, kVertexCount * 2 * sizeof(float),
                                    StorageUsage::Vertex);
    r.indices = new_ptr<StorageBuffer>(g_mgr, kIndexCount * sizeof(uint), StorageUsage::Index);
    r.draws = new_ptr<StorageBuffer>(g_mgr, kIndirectRecordBytes, StorageUsage::Indirect);
    r.pixels = new_ptr<StorageBuffer>(g_mgr, pixel_bytes);

    // Start from a known state so a readback of a buffer nobody wrote is
    // distinguishable from the compute output.
    vec<uint8_t> zero(kWords * sizeof(uint), 0);
    r.marker->write(zero.data(), zero.size());

    r.compute = new_ptr<Shader>(g_mgr);
    r.compute->loadShader("shaders/m3_setup.comp.spv", Shader::Compute);
    for (uint b = 0; b < 4; ++b)
        r.compute->addStorageBuffer(b, Shader::Compute);
    r.compute->buildComputePipeline();

    // The draw pipeline consumes the compute output: a vertex buffer of two
    // floats, the indices, and the marker SSBO (read-only, so no optional
    // fragmentStoresAndAtomics feature is needed).
    r.draw = new_ptr<Shader>(g_mgr);
    r.draw->loadShader("shaders/m3_triangle.vert.spv", Shader::Vertex);
    r.draw->loadShader("shaders/m3_triangle.frag.spv", Shader::Fragment);
    r.draw->addStorageBuffer(0, Shader::Fragment);
    r.draw->setVertexStride(2 * sizeof(float));
    VertexLayout layout;
    layout.push_back({"position", 0, 0, VertexInput::float2});
    r.draw->setVertexLayout(layout);
    // No depth relationship with the scene; also disables culling, so the
    // triangle's winding cannot make the test pass by drawing nothing.
    r.draw->setDepthTestEnabled(false);
    r.draw->build();

    // --- measurement --------------------------------------------------------
    // One pass over the three draws, at one frame size. A compositor resize
    // mid-pass invalidates the comparison of the pixel counts (the same triangle
    // covers a different number of pixels), so a pass that sees the size move
    // is discarded and repeated. The swapchain only settles once the window has
    // been mapped by the compositor, which is why this cannot just be a wait.
    vec<float> positions(kVertexCount * 2);
    vec<uint> index_data(kIndexCount);
    vec<uint> marker(kWords, 0);
    uint record[5] = {0};

    uint indirect_pixels = 0;
    uint direct_pixels = 0;
    uint decoy_pixels = 0;
    bool measured = false;

    for (uint attempt = 0; attempt < 6 && !measured; ++attempt) {
        // Keep the capture buffer in step with the swapchain extent.
        if (g_mgr->getFrameSize().x != r.frame_size.x ||
            g_mgr->getFrameSize().y != r.frame_size.y) {
            g_mgr->sync();
            r.frame_size = g_mgr->getFrameSize();
            const size_t bytes = (size_t)r.frame_size.x * r.frame_size.y * 4;
            r.pixels_cpu.assign(bytes, 0);
            r.pixels = new_ptr<StorageBuffer>(g_mgr, bytes);
        }

        const size_t bytes = (size_t)r.frame_size.x * r.frame_size.y * 4;
        bool stable = true;

        for (DrawVariant variant :
             {DrawVariant::Indirect, DrawVariant::DirectSameRange, DrawVariant::DirectFirstRange}) {
            win->update();
            g_mgr->update();
            if (g_mgr->getFrameSize().x != r.frame_size.x ||
                g_mgr->getFrameSize().y != r.frame_size.y) {
                stable = false;
                break;
            }

            auto cmd = record_frame(g_mgr, r, variant);
            if (!submit_and_wait(g_mgr, win, cmd)) {
                cmd.reset();
                stable = false;
                break;
            }
            cmd.reset();

            if (g_mgr->getFrameSize().x != r.frame_size.x ||
                g_mgr->getFrameSize().y != r.frame_size.y) {
                stable = false;
                break;
            }

            // Zeroed first: if the capture did not happen (a skipped copy), the
            // count is 0 and the test fails instead of reading a stale frame.
            std::memset(r.pixels_cpu.data(), 0, bytes);
            r.pixels->read(r.pixels_cpu.data(), bytes);
            const uint covered = countCoveredPixels(r.pixels_cpu);

            if (variant == DrawVariant::Indirect) {
                indirect_pixels = covered;
                r.marker->read(marker.data(), marker.size() * sizeof(uint));
                r.verts->read(positions.data(), positions.size() * sizeof(float));
                r.indices->read(index_data.data(), index_data.size() * sizeof(uint));
                r.draws->read(record, sizeof(record));
            } else if (variant == DrawVariant::DirectSameRange) {
                direct_pixels = covered;
            } else {
                decoy_pixels = covered;
            }
        }

        measured = stable;
        if (!stable)
            std::printf("ComputeIndirectTest: frame resized during the measurement, retrying\n");
    }

    if (!measured) {
        check.expect(false, "the measurement completed at a stable frame size", "");
    }

    // --- 1. the SSBO the compute shader filled ------------------------------
    uint bad = 0;
    for (uint i = 0; i < kWords; ++i)
        if (marker[i] != i * 3 + 7)
            ++bad;
    char detail[192];
    std::snprintf(detail, sizeof(detail), "(%u/%u marker words == i*3+7)", kWords - bad, kWords);
    check.expect(bad == 0, "compute wrote the SSBO", detail);

    // --- 2. the geometry the compute shader wrote, read back element-wise ---
    const float expected_pos[12] = {-0.95f, -0.95f, -0.85f, -0.95f, -0.90f, -0.85f,
                                    -0.60f, -0.60f, 0.60f,  -0.60f, 0.00f,  0.70f};
    const uint expected_idx[6] = {0, 1, 2, 3, 4, 5};
    const bool geom_ok =
        std::memcmp(positions.data(), expected_pos, sizeof(expected_pos)) == 0 &&
        std::memcmp(index_data.data(), expected_idx, sizeof(expected_idx)) == 0;
    check.expect(geom_ok, "compute wrote the vertex and index buffers");

    // --- 3. the record the indirect draw consumed ---------------------------
    const bool record_ok = record[0] == 3 && record[1] == 1 && record[2] == 3 && record[3] == 0 &&
                           record[4] == 0;
    std::snprintf(detail, sizeof(detail),
                  "(count=%u instances=%u firstIndex=%u vertexOffset=%u firstInstance=%u)",
                  record[0], record[1], record[2], record[3], record[4]);
    check.expect(record_ok, "compute wrote the indirect draw record", detail);

    // --- 4. the indirect draw rasterised something --------------------------
    std::snprintf(detail, sizeof(detail), "(%u covered pixels)", indirect_pixels);
    check.expect(indirect_pixels > 0, "drawIndexedIndirect() rendered the compute geometry", detail);

    // --- 5. indirect vs direct draw of the same range -----------------------
    std::snprintf(detail, sizeof(detail), "(indirect %u, direct %u)", indirect_pixels,
                  direct_pixels);
    check.expect(indirect_pixels > 0 && indirect_pixels == direct_pixels,
                 "the indirect draw matches the direct draw of the same range", detail);

    // --- 6. firstIndex really came from the buffer --------------------------
    std::snprintf(detail, sizeof(detail), "(firstIndex 3: %u pixels, firstIndex 0: %u pixels)",
                  indirect_pixels, decoy_pixels);
    check.expect(decoy_pixels > 0 && indirect_pixels > decoy_pixels * 4,
                 "firstIndex in the draw record was honoured", detail);

    if (!want_barrier) {
        // lavapipe (and any driver that serialises submission order) makes the
        // dispatch finish before the draw regardless, so the corruption the
        // barrier prevents is not observable there. Say so instead of pretending
        // the negative run proves something.
        std::printf("ComputeIndirectTest: NOTE barrier omitted (M3_NO_BARRIER); this driver "
                    "serialises the dispatch and the draw, so the readback is expected to match "
                    "anyway - the barrier test only fails on a driver that overlaps them\n");
    }

    r.release();
    g_mgr->cleanup();
    win->cleanup();
    Log::close();

    std::printf("ComputeIndirectTest: SUMMARY pass=%u fail=%u barrier=%u indirect_pixels=%u\n",
                check.passes, check.failures, (uint)want_barrier, indirect_pixels);
    return check.failures == 0 ? 0 : 1;
}
