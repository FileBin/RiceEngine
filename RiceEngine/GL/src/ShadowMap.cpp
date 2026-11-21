/*
 * ShadowMap.cpp
 */

#include "pch.h"

#include "Vulkan_API_code/api_GraphicsManager.hpp"
#include "Vulkan_API_code/api_ShadowMap_impl.inl"
#include <Rice/GL/ShadowMap.hpp>

NSP_GL_BEGIN

ShadowMap::ShadowMap(ptr<GraphicsManager> g_mgr, uint size)
    : GraphicsComponentBase(g_mgr), api_data(new ShadowMap_API_Data(get_api_data(), size)) {
    api_data->create();
}

ShadowMap::~ShadowMap() { cleanup(); }

void ShadowMap::cleanup() {
    if (!api_data)
        return;
    api_data->cleanup();
    delete api_data.release();
}

uint ShadowMap::getSize() const { return api_data ? api_data->size : 0; }

uint ShadowMap::getCasterCount() const { return api_data ? api_data->caster_count : 0; }

void ShadowMap::setCasterCount(uint count) {
    if (api_data)
        api_data->caster_count = count;
}

uint ShadowMap::readFilledTexelCount() {
    if (!api_data || !api_data->created)
        return 0;

    using namespace vk;

    GraphicsManager_API_data &d = get_api_data();
    d.sync(); // no frame in flight: the map is complete and idle

    const uint n = api_data->size * api_data->size;
    const DeviceSize bytes = (DeviceSize)n * sizeof(float);

    vk::Buffer staging;
    vk::DeviceMemory staging_memory;
    d.createBuffer(bytes, BufferUsageFlagBits::eTransferDst,
                   MemoryPropertyFlagBits::eHostVisible | MemoryPropertyFlagBits::eHostCoherent,
                   staging, staging_memory);

    CommandBufferAllocateInfo alloc{};
    alloc.commandPool = d.commandPool;
    alloc.level = CommandBufferLevel::ePrimary;
    alloc.commandBufferCount = 1;
    vk::CommandBuffer cmd = d.device.allocateCommandBuffers(alloc)[0];

    CommandBufferBeginInfo begin{};
    begin.flags = CommandBufferUsageFlagBits::eOneTimeSubmit;
    cmd.begin(begin);

    ImageMemoryBarrier to_src{};
    to_src.oldLayout = ImageLayout::eShaderReadOnlyOptimal;
    to_src.newLayout = ImageLayout::eTransferSrcOptimal;
    to_src.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    to_src.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    to_src.image = api_data->image;
    to_src.subresourceRange = ImageSubresourceRange(ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
    to_src.srcAccessMask = AccessFlagBits::eShaderRead;
    to_src.dstAccessMask = AccessFlagBits::eTransferRead;
    cmd.pipelineBarrier(PipelineStageFlagBits::eFragmentShader,
                        PipelineStageFlagBits::eTransfer, DependencyFlags{}, nullptr, nullptr,
                        to_src);

    BufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource =
        ImageSubresourceLayers(ImageAspectFlagBits::eDepth, 0, 0, 1);
    region.imageOffset = Offset3D(0, 0, 0);
    region.imageExtent = Extent3D(api_data->size, api_data->size, 1);
    cmd.copyImageToBuffer(api_data->image, ImageLayout::eTransferSrcOptimal, staging, 1,
                          &region);

    ImageMemoryBarrier to_read = to_src;
    to_read.oldLayout = ImageLayout::eTransferSrcOptimal;
    to_read.newLayout = ImageLayout::eShaderReadOnlyOptimal;
    to_read.srcAccessMask = AccessFlagBits::eTransferRead;
    to_read.dstAccessMask = AccessFlagBits::eShaderRead;
    cmd.pipelineBarrier(PipelineStageFlagBits::eTransfer, PipelineStageFlagBits::eFragmentShader,
                        DependencyFlags{}, nullptr, nullptr, to_read);

    cmd.end();

    SubmitInfo submit{};
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd;
    auto res = d.graphicsQueue.submit(1, &submit, nullptr);
    THROW_VK_EX_IF_BAD(res);
    d.graphicsQueue.waitIdle();
    d.device.freeCommandBuffers(d.commandPool, 1, &cmd);

    void *mapped = d.device.mapMemory(staging_memory, 0, bytes);
    uint filled = 0;
    const float *depths = (const float *)mapped;
    for (uint i = 0; i < n; ++i)
        if (depths[i] < 1.f - 1e-6f) // 1.0 is the clear value = "nothing here"
            ++filled;
    d.device.unmapMemory(staging_memory);

    d.device.destroy(staging);
    d.device.free(staging_memory);

    return filled;
}

Matrix4x4 ShadowMap::buildLightViewProj(Vector3 light_direction, Vector3 center,
                                        dbl shadow_distance) {
    // Row-vector convention throughout: a point is transformed as
    // p * view * projection, i.e. the rows of the view matrix are the light
    // space basis vectors and the translation sits in the fourth column.

    Vector3 forward = light_direction;
    if (forward.length() < 1e-9)
        forward = Vector3{0, -1, 0}; // no direction given: straight down
    forward.normalize();

    // The up vector of the light space. A sun that points straight down makes
    // the usual world-up degenerate, so those cases use +z as the hint instead
    // (that also keeps the light space roll stable for a Minecraft-like world,
    // where the sun sweeps across the sky).
    const Vector3 world_up{0, 1, 0};
    Vector3 up_hint = world_up;
    if (std::abs(Vector3::dot(forward, world_up)) > .99)
        up_hint = Vector3{0, 0, 1};

    Vector3 right = Vector3::cross(up_hint, forward);
    right.normalize();
    Vector3 up = Vector3::cross(forward, right);
    up.normalize();

    const dbl half = std::max(shadow_distance, 1.);
    // 0 would put the near plane exactly at the eye; a hair in front of it keeps
    // the light's own position out of the depth range.
    const dbl near_plane = half * .001;
    const dbl far_plane = half * 2.;

    // Light view: world -> light space, eye `half` units behind the centre.
    const Vector3 eye = center - forward * half;
    const Vector3 t = eye; // for readability of the translation column
    Matrix4x4 view{right.x,
                   right.y,
                   right.z,
                   -Vector3::dot(t, right),
                   up.x,
                   up.y,
                   up.z,
                   -Vector3::dot(t, up),
                   forward.x,
                   forward.y,
                   forward.z,
                   -Vector3::dot(t, forward),
                   0,
                   0,
                   0,
                   1};

    // Orthographic projection of the square [center +- half] in light space.
    // x, y map to [-1, 1] and z maps to [0, 1] (Vulkan clip space), so lPos.z is
    // on the same scale as the depth stored in the map and the shader can
    // compare the two directly.
    //
    // Matrix4x4::ortographic() is deliberately not used here: it maps z to
    // [-1, 1] (the OpenGL/DirectX-old convention), which is exactly what the
    // ported PCF (bias - lightSpaceZ against the stored depth) cannot work with.
    Matrix4x4 projection{
        1. / half, 0,          0, 0, //
        0,         1. / half,  0, 0, //
        0,         0,          1. / (far_plane - near_plane), -near_plane / (far_plane - near_plane),
        0,         0,          0, 1};

    return view * projection;
}

NSP_GL_END
