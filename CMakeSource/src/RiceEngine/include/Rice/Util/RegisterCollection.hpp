#pragma once

#include "../stdafx.hpp"

template<typename ElemT, typename Id_t = size_t>
class RegisterCollection {
public:
	typedef SmartPtr<ElemT> ptr_t;
private:
	vec<ptr_t> collection{};
	vec<Id_t> emptyPositions{};
public:

	RegisterCollection() {
		emptyPositions.reserve(0xfff);
	}

	const vec<ptr_t> asStdVector() const {
		vec<ptr_t> vec{};
		auto n = collection.size();
		vec.reserve(n);
		for (Id_t i = 0; i < n; i++) {
			const auto& elem = collection[i];
			if (elem != nullptr)
				vec.push_back(elem);
		}
		return vec;
	}


	const vec<ptr_t>& getCollectionWithGaps() const {
		return collection;
	}

	ptr_t getElemAt(Id_t idx) {
		return collection[idx];
	}

	Id_t registerPtr(ptr_t pObject) {
		if (emptyPositions.empty()) {
			collection.push_back(pObject);
			return collection.size() - 1;
		} else {
			auto idx = emptyPositions.back();
			collection[idx] = pObject;
			emptyPositions.pop_back();
			return idx;
		}
	}

	Id_t registerObj(ElemT object) {
		return registerPtr(new ElemT(object));
	}

	void unregister(Id_t index) {
		collection[index] = nullptr;
		emptyPositions.push_back(index);
	}
};
