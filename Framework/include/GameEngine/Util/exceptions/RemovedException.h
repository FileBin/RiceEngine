#pragma once

#include "exception.h" 

#define THROW_REMOVED_EXCEPTION(pointer) throw Game::RemovedException(pointer, "", __LINE__, __FILE__)

namespace Game {
	class RemovedException : public Exception {
	private:
		void* ptr;
	public:
		RemovedException(void* whatPtr, int line_, const char* file_) : Exception("Pointer was removed", line_, file_) {
			ptr = whatPtr;
		}

		void* GetPointer() { return ptr; }
	};
}
