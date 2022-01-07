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
			std::string info = "";
			info.append("TypeInfo: ");
			info.append(GetTypeInfo().name());
			info.append("\n");
			info.append("Pointer: ");
			info.append(std::to_string((ULONG)GetPointer()));
			info.append("\n");
			SetInfo(info.c_str());
		}

		std::type_index GetTypeInfo() { return type; }

		void* GetPointer() { return ptr; }
	};
}
