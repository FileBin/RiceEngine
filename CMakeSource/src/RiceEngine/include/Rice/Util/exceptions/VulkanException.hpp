#include "Exception.hpp"

#define THROW_VK_EX(res) throw ::Rice::VulkanException(((vk::Result)(res)), __LINE__, __FILE__)


#define THROW_VK_EX_IF_BAD(res) if(((vk::Result)(res))!=vk::Result::eSuccess) throw ::Rice::VulkanException(((vk::Result)(res)), __LINE__, __FILE__)


NSP_ENGINE_BEGIN

class VulkanException : public Exception {
public:

	VulkanException(vk::Result res, int line, const char* file) : Exception("VulkanException", line, file) {
		SetInfo(_getInfo(res));
	}
private:
	String _getInfo(vk::Result res){
		switch (res) {
			case vk::Result::eSuboptimalKHR:
				return "SubOptimalKHR";
			case vk::Result::eErrorDeviceLost:
				return "ErrorDeviceLost!";
				//TODO make other vk error cases
			default:
				return "Unknown exception!";
		}
	}
};

NSP_ENGINE_END
