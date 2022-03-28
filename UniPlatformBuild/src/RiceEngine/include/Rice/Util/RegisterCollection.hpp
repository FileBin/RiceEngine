#pragma once

#include <vector>
#include <queue>

template<typename PtrT = void*>
class RegisterCollection {
private:
	_STD vector<PtrT> collection{};
	_STD vector<size_t> emptyPositions{};
public:

	RegisterCollection() {
		emptyPositions.reserve(0xfff);
	}

	const _STD vector<PtrT> AsStdVector() const {
		_STD vector<PtrT> vec{};
		auto n = collection.size();
		vec.reserve(n);
		for (size_t i = 0; i < n; i++) {
			const auto& elem = collection[i];
			if (elem != nullptr)
				vec.push_back(elem);
		}
		return vec;
	}


	const _STD vector<PtrT>& GetCollectionWithGaps() const {
		return collection;
	}

	PtrT GetElemAt(size_t idx) {
		return collection[idx];
	}

	size_t Register(PtrT pObject) {
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

	void Unregister(size_t index) {
		collection[index] = nullptr;
		emptyPositions.push_back(index);
	}
};
