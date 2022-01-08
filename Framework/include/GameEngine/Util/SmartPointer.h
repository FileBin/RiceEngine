#pragma once

#include "exceptions.h"
#include "../stdafx.h"
#include <mutex>

namespace Game {
	class g_Vars {
	public:
		static std::mutex smartPtrConstructorMutex;
	};
}

template<typename T = void>
class /*Not*/SmartPtr {
private:
	//std::mutex m_mutex;
	std::shared_ptr<T*> ppObject;
public:

	SmartPtr(const SmartPtr<T>& other) {
		//std::lock_guard<std::mutex> lock(Game::g_Vars::smartPtrConstructorMutex);
		ppObject = other.ppObject;
	}

	SmartPtr(T* obj = nullptr) {
		ppObject = std::make_shared<T*>(obj);
	}

	~SmartPtr() {
		//std::lock_guard<std::mutex> lock(Game::g_Vars::smartPtrConstructorMutex);
	}

	bool IsNull() {
		if (!ppObject.get())
			THROW_REMOVED_EXCEPTION(ppObject.get());
		return *ppObject.get() == nullptr;
	}

	void Release() {
		if (!ppObject.get())
			THROW_REMOVED_EXCEPTION(ppObject.get());
		T* pointer = *ppObject.get();
		//std::lock_guard<std::mutex> lock(m_mutex);
		*ppObject.get() = nullptr;
		delete pointer;
	}

	T* Get() const {
		if(!ppObject.get())
			THROW_REMOVED_EXCEPTION(ppObject.get());
		return *ppObject.get();
	}

	T** GetAddress() {
		return ppObject.get();
	}

	T* operator->() {
		if (!ppObject.get())
			THROW_REMOVED_EXCEPTION(ppObject.get());
		if (!*ppObject.get())
			THROW_NULL_PTR_EXCEPTION(*ppObject.get());
		return *ppObject.get();
	}

	const SmartPtr<T>& operator=(const SmartPtr<T>& other) {
		ppObject = other.ppObject;
		return *this;
	}

	T& operator*() {
		return **ppObject.get();
	}
};

template<typename T>
bool operator==(const SmartPtr<T>& a, const SmartPtr<T>& b) {
	return a.Get() == b.Get();
}