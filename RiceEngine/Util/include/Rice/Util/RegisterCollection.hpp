/*
 * RegisterCollection.hpp
 *
 *  Created on: Apr 29, 2022
 *      Author: filebin
 */

#pragma once

#include "Interfaces.hpp"
#include "Rice/Util/namespaces.h"

NSP_UTIL_BEGIN

template <typename ElemT, typename Id_t = uint> class RegisterCollection {
  public:
    typedef ptr<ElemT> ptr_t;

  private:
    vec<ptr_t> collection{};
    vec<Id_t> emptyPositions{};

  public:
    RegisterCollection() { emptyPositions.reserve(0xfff); }

    const vec<ptr_t> &getCollection() const {
        vec<ptr_t> list{};
        auto n = collection.size();
        list.reserve(n);
        for (Id_t i = 0; i < n; i++) {
            const auto &elem = collection[i];
            if (elem != nullptr)
                list.push_back(elem);
        }
        return list;
    }

    const vec<ptr_t> &getCollectionWithGaps() const { return collection; }

    ptr_t getElemAt(Id_t idx) { return collection[idx]; }

    Id_t registerPtr(ptr_t pObject) {
        if (emptyPositions.empty()) {
            collection.push_back(pObject);
            return collection.size() - 1;
        } else {
            uint idx = emptyPositions.size() - 1;
            collection[idx] = pObject;
            emptyPositions.pop_back();
            return idx;
        }
    }

    ElemT& get(Id_t idx) { return *collection[idx]; }
    ptr_t getPtr(Id_t idx) { return collection[idx]; }

    Id_t registerObj(ElemT object) {
        return registerPtr(ptr_t{new ElemT(object)});
    }

    void unregister(Id_t index) {
        collection[index] = nullptr;
        emptyPositions.push_back(index);
    }

    void cleanup() {
        collection.clear();
        emptyPositions.clear();
    }
};

NSP_UTIL_END
