#pragma once
#include "../stdafx.hpp"

#include "Exceptions.hpp"

template<typename T>
class AutoPtr;

template<typename T = void>
class SmartPtr {
protected:
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

	template<typename... Args>
	static SmartPtr<T> create(Args... args) {
		return new T(args...);
	}

	virtual ~SmartPtr() {}

	bool isNull() const {
		if (ppObject.get() == nullptr)
			THROW_NULL_PTR_EXCEPTION(ppObject.get());
		return *ppObject == nullptr;
	}

	bool isNotNull() const {
		if (ppObject.get() == nullptr)
			THROW_NULL_PTR_EXCEPTION(ppObject.get());
		return *ppObject != nullptr;
	}

	void release() {
		if (isNotNull()) {
			T* pointer = *ppObject;
			delete pointer;
			*ppObject = nullptr;
		}
	}

	void releaseArray() {
		if (isNotNull()) {
			T* pointer = *ppObject;
			*ppObject = nullptr;
			delete[] pointer;
		}
	}

	T* get() const {
		return *ppObject;
	}

	T** getAddress() const {
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
	return a.isNull();
}

template<typename T>
class AutoPtr : public SmartPtr<T> {
public:
	AutoPtr(const SmartPtr<T>& other) : SmartPtr<T>(other) {}
	AutoPtr(const AutoPtr<T>& other) = delete;

	template<typename... Args>
	AutoPtr(Args... args) {
		return new T(args...);
	}

	AutoPtr(T* obj) : SmartPtr<T>(obj) {}

	~AutoPtr() { this->release(); }
};
