#pragma once

#include "exception.h"
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
			throw Game::exception("Pointer is not initialized or deleted!", 27, L"Release() SmartPointer.h");
		T* pointer = ppObject[0];
		ppObject[0] = nullptr;
		delete pointer;
	}

	T operator->() {
		if (!ppObject)
			throw Game::exception("Pointer is not initialized or deleted!", 32, L"operator->() SmartPointer.h");
		if(!ppObject[0])
			throw Game::exception("NullPointerExeption", 34, L"operator->() SmartPointer.h");
		return ppObject[0][0];
	}
};