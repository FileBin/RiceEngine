#pragma once

#include "Exception.hpp"

#define THROW_NULL_PTR_EXCEPTION(pointer) throw NSP_UTIL::NullPtrException(pointer, __LINE__, __FILE__)
#define THROW_IF_NULL(pointer) if(!pointer) throw NSP_UTIL::NullPtrException(pointer, __LINE__, __FILE__)

NSP_UTIL_BEGIN

class NullPtrException : public Exception {
private:
	std::type_index type;
	void* ptr;

	void __setInfo();

public:
	template<typename ptrT = void*>
	NullPtrException(ptrT whatPtr, int line_, const char* file_) : Exception("Pointer is null", line_, file_), type(typeid(whatPtr)) {
		ptr = (void*)whatPtr;
		__setInfo();
	}

	std::type_index getTypeInfo() { return type; }

	void* getPointer() { return ptr; }
};

NSP_UTIL_END
