#pragma once

#define THROW_VK_EX(res) throw ::Rice::Graphics::VulkanException(((vk::Result)(res)), __LINE__, __FILE__)


#define THROW_VK_EX_IF_BAD(res) if(((int)(res))<0) THROW_VK_EX(res)


NSP_GL_BEGIN

class VulkanException : public Util::Exception {
public:

	VulkanException(vk::Result res, int line, const char* file) : Exception("VulkanException", line, file) {
		setInfo(_getInfo(res));
	}
private:
	String _getInfo(vk::Result res){
		switch (res) {
			case vk::Result::eSuboptimalKHR:
				return "SubOptimalKHR";
			case vk::Result::eErrorDeviceLost:
				return "ErrorDeviceLost!";

			case vk::Result::eErrorOutOfDateKHR:
				return "ErrorOutOfDateKHR!";
				//TODO make other vk error cases
			default:
				return "Unknown exception!";
		}
	}
};

NSP_GL_END
