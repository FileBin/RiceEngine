#pragma once

#include "exceptions.h"
#include "../stdafx.h"
#include <mutex>


template<typename T = void>
class /*Not*/SmartPtr {
private:
	std::shared_ptr<T*> ppObject;
public:

	SmartPtr(const SmartPtr<T>& other) {
		if (other.ppObject.get())
			ppObject = other.ppObject;
		else
			THROW_NULL_PTR_EXCEPTION(other.ppObject.get());
	}

	SmartPtr(T* obj = nullptr) {
		ppObject = std::make_shared<T*>(obj);
	}

	~SmartPtr() {
	}

	bool IsNull() const {
		if (ppObject.get() == nullptr)
			THROW_NULL_PTR_EXCEPTION(ppObject.get());
		return *ppObject == nullptr;
	}

	void Release() {
		if (!IsNull()) {
			T* pointer = *ppObject;
			*ppObject = nullptr;
			delete pointer;
		}
	}

	void ReleaseArray() {
		if (!IsNull()) {
			T* pointer = *ppObject;
			*ppObject = nullptr;
			delete[] pointer;
		}
	}

	T* Get() const {
		return *ppObject;
	}

	T** GetAddress() const {
		return ppObject.get();
	}

	T* operator->() const {
		if (!*ppObject)
			THROW_NULL_PTR_EXCEPTION(*ppObject);
		return *ppObject;
	}

	const SmartPtr<T>& operator=(const SmartPtr<T>& other) {
		ppObject = other.ppObject;
		return *this;
	}

	T& operator*() {
		return **ppObject;
	}
};

template<typename T>
bool operator==(const SmartPtr<T>& a, const SmartPtr<T>& b) {
	return a.Get() == b.Get();
}

template<typename T>
bool operator==(const SmartPtr<T>& a, _STD nullptr_t b) {
	return a.IsNull();
}