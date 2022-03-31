#include "../stdafx.hpp"
class Texture2D;
using pTexture2D = SmartPtr<Texture2D>;
#pragma once

NSP_GL_BEGIN

class Texture2D {
public:
	Texture2D(String filename, vk::Device device, vk::SamplerAddressMode u_mode, vk::SamplerAddressMode v_mode, vk::Filter filter);
	Texture2D(vk::ImageView ppShaderResource, vk::Device device);

	vk::Sampler GetSampler() { return sampler; }
	vk::ImageView GetResource() { return image; }

	~Texture2D();
private:
	vk::Sampler sampler;
	vk::ImageView image;
};

NSP_GL_END
