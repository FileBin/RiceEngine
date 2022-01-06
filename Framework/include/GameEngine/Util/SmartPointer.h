#pragma once

#include "exceptions.h"
#include "../stdafx.h"

template<typename T = std::nullptr_t>
class Ptr {
private:
	T** ppObject;
public:

	Ptr(const Ptr& other) {

	}

	Ptr(T* obj) {
		ppObject = new T*[1];
		ppObject[0] = obj;
	}

	~Ptr() {
		delete ppObject;
		ppObject = nullptr;
	}

	void Release() {
		if (!ppObject)
			THROW_REMOVED_EXCEPTION(ppObject);
		T* pointer = ppObject[0];
		ppObject[0] = nullptr;
		delete pointer;
	}

	T* Get() {
		return ppObject[0];
	}

	T** GetAddress() {
		return ppObject;
	}

	T* operator->() {
		if (!ppObject)
			THROW_REMOVED_EXCEPTION(ppObject);
		if(!ppObject[0])
			THROW_NULL_PTR_EXCEPTION(*this);
		return ppObject[0];
	}
};