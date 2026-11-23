/*
 * CommandBuffer.hpp
 *
 *  Created on: 26 April 2022
 *      Author: FileBinsLapTop
 */

#include "Rice/Util/Interfaces.hpp"
#include "stdafx.hpp"
#include <Rice/Math/Vectors.hpp>

NSP_GL_BEGIN

class CommandBuffer;

struct CommandBuffer_API_data;

NSP_GL_END

#pragma once

#include "Buffer.hpp"
#include "GraphicsComponentBase.hpp"
#include "GraphicsManager.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "ShadowMap.hpp"
#include "StorageBuffer.hpp"
#include "UniformBuffer.hpp"
#include "VertexBuffer.hpp"

NSP_GL_BEGIN

/*
 * Stages a CommandBuffer::memoryBarrier() can name, as a bit mask (they are
 * combined with operator|). The set the GPU mesher needs is small on purpose
 * (docs/tetrahedral-mesh.md §6.4): a compute pass writes buffers, and the
 * vertex fetch, the indirect draw and the next dispatch read them.
 */
enum class BarrierStage : uint {
    None = 0,
    TopOfPipe = 1 << 0,
    ComputeShader = 1 << 1,
    VertexInput = 1 << 2,
    DrawIndirect = 1 << 3,
    Transfer = 1 << 4,
    FragmentShader = 1 << 5,
    AllGraphics = 1 << 6,
    AllCommands = 1 << 7,
};
inline BarrierStage operator|(BarrierStage a, BarrierStage b) {
    return BarrierStage(uint(a) | uint(b));
}

/* Memory accesses the barrier makes available/visible, as a bit mask. */
enum class BarrierAccess : uint {
    None = 0,
    ShaderWrite = 1 << 0,
    ShaderRead = 1 << 1,
    VertexAttributeRead = 1 << 2,
    IndexRead = 1 << 3,
    IndirectCommandRead = 1 << 4,
    TransferRead = 1 << 5,
    TransferWrite = 1 << 6,
};
inline BarrierAccess operator|(BarrierAccess a, BarrierAccess b) {
    return BarrierAccess(uint(a) | uint(b));
}

class CommandBuffer : public GraphicsComponentBase {
  public:
    friend class GraphicsManager;
    friend struct CommandBuffer_API_data;
    class Command {
      public:
        enum Type {
            BeginRenderPass,
            ClearRenderTarget,
            ExecuteCommandBuffer,
            EndRenderPass,
            BindVertexBuffer,
            BindIndexBuffer,
            SetShader,
            Draw,
            DrawIndexed,
            PushConstants,
            BindUniformBuffer,
            // Depth-only casters pass: begin/end the shadow map's render pass
            // and bind the map as a combined image sampler for the lit draws.
            BeginShadowPass,
            EndShadowPass,
            BindShadowMap,
            // Compute: an SSBO binding for the set, a dispatch, and a global
            // memory barrier between a compute write and the read that follows.
            BindStorageBuffer,
            Dispatch,
            PipelineBarrier,
            // Game-owned geometry: an indirect draw whose records live in a
            // buffer (written by compute, §6.3), and an explicit-range draw.
            DrawIndexedIndirect,
            DrawIndexedRange,
            // Copy the frame's colour image into a host-visible buffer.
            CaptureColor,
            // GPU-side buffer -> buffer copy (the double-buffered page publish
            // uses it: a page it can no longer write is brought up to date with
            // one recorded command instead of a host memcpy of the arena).
            CopyBuffer,
        } cmd;

        struct Arg {
            Arg *next = 0;
            virtual size_t getSize() const = 0;
            virtual void *getData() const = 0;
            virtual Arg *clone() const = 0;
            virtual ~Arg(){};

        } *arg_chain;

        ICleanable *additional_data = nullptr;

        struct ArgIterator {
            Arg *current;

            ArgIterator(Arg *arg) { current = arg; }

            ArgIterator &operator++() {
                current = current->next;
                return *this;
            }

            Arg &operator*() { return *current; }

            ArgIterator operator++(int) {
                ArgIterator tmp = *this;
                ++(*this);
                return tmp;
            }
        };

        template <typename T> struct TypedArg : public Arg {
            T data;
            size_t getSize() const override { return sizeof(T); }
            void *getData() const override { return (void *)&data; }

            TypedArg() = default;
            TypedArg(const T &d) : data(d){};

            Arg *clone() const override {
                TypedArg<T> *arg = new TypedArg<T>(data);
                if (next)
                    arg->next = next->clone();
                return arg;
            }
            ~TypedArg() {
                if (next)
                    delete next;
            }
        };

      private:
        template <typename... ArgsT> struct __convert_args {
            static Arg *result() { return nullptr; };
        };

        template <typename FirstT, typename... ArgsT> struct __convert_args<FirstT, ArgsT...> {
            static Arg *result(FirstT first, ArgsT... args) {
                TypedArg<FirstT> *t_arg = new TypedArg<FirstT>(first);
                t_arg->next = __convert_args<ArgsT...>::result(args...);
                return t_arg;
            };
        };

      public:
        template <typename... ArgsT> Command(Type cmd, ArgsT... args) : cmd(cmd) {
            arg_chain = __convert_args<ArgsT...>::result(args...);
        }

        Command(const Command &other) {
            cmd = other.cmd;
            arg_chain = other.arg_chain->clone();
        }

        ~Command() {
            if (arg_chain)
                delete arg_chain;
            if (additional_data) {
                additional_data->cleanup();
                delete additional_data;
            }
        }
    };

  protected:
    Util::EventRegistration resizeReg;
    uptr<CommandBuffer_API_data> api_data;

    vec<ptr<Command>> commands;
    vec<bool> need_recreate = {};

    void needRecreate(bool value = true);

  public:
    CommandBuffer(ptr<GraphicsManager> g_mgr, bool secondary = false);
    ~CommandBuffer() override;

    void clear();

    void drawVertices(uint count);
    void drawIndexed(ptr<IndexBuffer> indexBuffer);
    void setActiveShader(ptr<Shader> shader, bool ghost = false);

    void beginRenderPass(Util::Rect rect, bool secondary = false);
    void clearRenderTarget(Vector3f color, float depth);
    void executeCommandBuffer(ptr<CommandBuffer> cmd);
    void endRenderPass();

    /*
     * Depth-only casters pass. `beginShadowPass` starts the shadow map's own
     * render pass (single D32_SFLOAT attachment, cleared to 1.0, no colour
     * target), so it has to be recorded OUTSIDE the window's render pass and
     * before it. Every draw recorded in between goes into the map; the pass must
     * be closed with endShadowPass() before the main pass begins, which also
     * leaves the map in the shader-read layout the scene shaders sample it in.
     */
    void beginShadowPass(ptr<ShadowMap> shadow_map);
    void endShadowPass();

    /* Set 0, binding `binding`: the shadow map as a combined image sampler. */
    void bindShadowMap(ptr<ShadowMap> shadow_map, uint binding);

    void bindVertexBuffer(ptr<Buffer> buffer);
    void bindIndexBuffer(ptr<Buffer> buffer);
    void bindUniformBuffer(ptr<UniformBuffer> uniformBuffer, uint binding);

    /*
     * Set 0, binding `binding`: a storage buffer (SSBO) for the draw or
     * dispatch that follows. The buffer must have been created with
     * StorageBuffer, and the shader must have declared the binding through
     * Shader::addStorageBuffer() with the same stage.
     */
    void bindStorageBuffer(ptr<StorageBuffer> buffer, uint binding);

    /*
     * Runs a compute pipeline: binds it, fills set 0 from the storage buffers
     * bound above and dispatches ceil(work / local_size) groups. `shader` must
     * have been built with Shader::buildComputePipeline().
     *
     * A dispatch cannot be recorded inside a render pass; record it before
     * beginRenderPass() and put a memoryBarrier() between it and the draw that
     * reads what it wrote.
     */
    void dispatch(ptr<Shader> shader, uint groupCountX, uint groupCountY = 1,
                  uint groupCountZ = 1);

    /*
     * One global memory barrier: everything the stages in `srcStage` wrote with
     * one of `srcAccess` becomes available, and visible to `dstStage` reading
     * it with `dstAccess`. The compute-write -> vertex/index/indirect-read
     * pattern of docs/tetrahedral-mesh.md §6.4:
     *
     *   memoryBarrier(BarrierStage::ComputeShader, BarrierAccess::ShaderWrite,
     *                 BarrierStage::VertexInput | BarrierStage::DrawIndirect,
     *                 BarrierAccess::VertexAttributeRead | BarrierAccess::IndexRead |
     *                     BarrierAccess::IndirectCommandRead);
     *
     * Submission order alone does not make that safe - without the barrier the
     * draw is free to fetch the buffer before the dispatch that fills it.
     */
    void memoryBarrier(BarrierStage srcStage, BarrierAccess srcAccess, BarrierStage dstStage,
                       BarrierAccess dstAccess);

    /*
     * Indexed draw whose VkDrawIndexedIndirectCommand records are read from
     * `draws` (a StorageBuffer created with StorageUsage::Indirect), starting
     * at record `firstDraw`. The counts and the first index are whatever the
     * buffer holds - the CPU does not need to know them, which is what lets a
     * compute pass decide how much geometry there is (§6.3).
     */
    void drawIndexedIndirect(ptr<Buffer> draws, uint firstDraw = 0, uint drawCount = 1);

    /*
     * Indexed draw of an explicit range of the bound index/vertex buffers -
     * the direct counterpart of drawIndexedIndirect(), for comparison and for
     * a fallback when the counts are known on the CPU.
     */
    void drawIndexed(uint indexCount, uint firstIndex, int32_t vertexOffset = 0);

    /*
     * Test/diagnostic: copies the frame's colour image into `dst`, which must
     * be host visible and hold at least width * height * 4 bytes
     * (GraphicsManager::getFrameSize()). Record it after endRenderPass(): the
     * image is transitioned PresentSrc -> TransferSrcOptimal -> PresentSrc
     * inside the frame, so the presentation engine never sees it in an
     * intermediate layout. GraphicsManager::sync() then makes the bytes
     * readable with StorageBuffer::read().
     */
    void captureColor(ptr<Buffer> dst);

    /*
     * GPU-side copy: one vkCmdCopyBuffer of `size` bytes from `src` to `dst`.
     * Both buffers are created with eTransferSrc | eTransferDst (see
     * StorageBuffer), so any two storage/vertex/index buffers pair. The bytes
     * move on the GPU and the host never maps either buffer, so the cost is a
     * single command however large the region is. Clamped to both buffers (a
     * region that no longer fits is skipped rather than recorded out of
     * bounds).
     *
     * It is a Transfer-stage command, so a memoryBarrier() pairs it with the
     * stage that wrote `src` (ShaderWrite -> TransferWrite/TransferRead) and
     * with the dispatch that later writes `dst` (TransferWrite -> ShaderWrite).
     *
     * One struct rather than five parameters: a page publish records hundreds of
     * these, and every command argument is its own allocation.
     */
    struct BufferCopy {
        ptr<Buffer> src;
        ptr<Buffer> dst;
        std::uint64_t srcOffset = 0;
        std::uint64_t dstOffset = 0;
        std::uint64_t size = 0;
    };
    void copyBuffer(const BufferCopy &copy);

    template <typename T>
    void pushConstants(T data, ptr<Shader> shader, Shader::Type stage = Shader::Vertex) {
        commands.push_back(
            new_ref<Command>(Command::PushConstants, shader, stage, (uint)sizeof(data), data));
    }

    void cleanup() override;

    void update();
    void build();
    void buildAll();
};
NSP_GL_END
