#pragma once

#include "exception.h" 

#include <typeinfo>
#include <typeindex>

#define THROW_NULL_PTR_EXCEPTION(pointer) throw Game::NullPtrException(pointer, __LINE__, __FILE__)

namespace Game {

	class NullPtrException : public Exception {
	private:
		std::type_index type;
		void* ptr;
	public:
		template<typename ptrT = void*>
		NullPtrException(ptrT whatPtr, int line_, const char* file_) : Exception("Pointer is null", line_, file_), type(typeid(whatPtr)) {
			ptr = (void*)whatPtr;
			std::string info = std::format("TypeInfo: {}\nPointer: {:#x}\n", GetTypeInfo().name(), (DWORD64)GetPointer());
			SetInfo(info);
		}

		std::type_index GetTypeInfo() { return type; }

		void* GetPointer() { return ptr; }
	};
}
