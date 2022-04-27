/*
 * api_Shader.hpp
 *
 *  Created on: Apr 27, 2022
 *      Author: root
 */

#pragma once

#include "VulkanHelper.hpp"

NSP_GL_BEGIN

struct Shader_API_Data {
	vk::ShaderModule vertexShader, fragmentShader, geometryShader;
	vk::Pipeline pipeline;
	vk::PipelineLayout layout;
};

NSP_GL_END
