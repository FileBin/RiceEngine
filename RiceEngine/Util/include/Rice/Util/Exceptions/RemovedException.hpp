#pragma once

#include <BetterCpp/Exceptions/Exception.hpp>

#define THROW_REMOVED_EXCEPTION(pointer) throw ::Rice::RemovedException(pointer, __LINE__, __FILE__)

NSP_ENGINE_BEGIN
class RemovedException : public Exception {
private:
	std::type_index type;
	void* ptr;
public:
	template<typename ptrT = void*>
	RemovedException(ptrT whatPtr, int line_, const char* file_) : Exception("Pointer was removed", line_, file_), type(typeid(whatPtr)) {
		ptr = (void*)whatPtr;
		String info = fmt::format("TypeInfo: {}\nPointer: {:#x}\n", GetTypeInfo().name(), (long long)GetPointer()).c_str(); // @suppress("Invalid arguments")
		SetInfo(info);
	}

	std::type_index GetTypeInfo() { return type; }
	void* GetPointer() { return ptr; }
};
NSP_ENGINE_END
