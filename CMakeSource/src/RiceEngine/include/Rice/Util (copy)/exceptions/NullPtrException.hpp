#pragma once

#include "Exception.hpp" 

#define THROW_NULL_PTR_EXCEPTION(pointer) throw ::Rice::NullPtrException(pointer, __LINE__, __FILE__)

NSP_ENGINE_BEGIN

class NullPtrException : public Exception {
private:
	_STD type_index type;
	void* ptr;
public:
	template<typename ptrT = void*>
	NullPtrException(ptrT whatPtr, int line_, const char* file_) : Exception("Pointer is null", line_, file_), type(typeid(whatPtr)) {
		ptr = (void*)whatPtr;
		String info = fmt::format(L"TypeInfo: {}\nPointer: {:#x}\n", String(getTypeInfo().name()), (num)getPointer()); // @suppress("Invalid arguments")
		SetInfo(info);
	}

	_STD type_index getTypeInfo() { return type; }

	void* getPointer() { return ptr; }
};

NSP_ENGINE_END
