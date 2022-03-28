#pragma once

#include "Exception.hpp" 

#define THROW_NULL_PTR_EXCEPTION(pointer) throw ::Rice::NullPtrException(pointer, __LINE__, __FILE__)

NSP_ENGINE_BEGIN

class NullPtrException : public Exception {
private:
	std::type_index type;
	void* ptr;
public:
	template<typename ptrT = void*>
	NullPtrException(ptrT whatPtr, int line_, const char* file_) : Exception("Pointer is null", line_, file_), type(typeid(whatPtr)) {
		ptr = (void*)whatPtr;
		std::string info = fmt::format("TypeInfo: {}\nPointer: {:#x}\n", GetTypeInfo().name(), (num)GetPointer());
		SetInfo(info);
	}

	std::type_index GetTypeInfo() { return type; }

	void* GetPointer() { return ptr; }
};

NSP_ENGINE_END
