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

/*
 * A dense-ish slot vector with a free list.
 *
 * Invariants (relied on by SceneRender's mesh queues):
 *  - `collection` holds the live elements and the holes (nullptr). A slot index
 *    is stable for as long as the element stays registered, so the id handed
 *    out by registerPtr can be stored and used later to unregister.
 *  - `emptyPositions` contains exactly the indices whose slot is currently
 *    nullptr, each index at most once. It behaves as a stack: registerPtr pops
 *    the last freed slot, so holes are reused LIFO and every hole is used
 *    exactly once before the vector has to grow.
 *  - Consequently registerPtr writes to `emptyPositions.back()`, i.e. to a
 *    *collection* index. `emptyPositions.size()-1` is an index into the free
 *    list and must never be used as a collection index — doing so overwrote
 *    slot 0 and left the freed slot empty (a silently lost element plus a
 *    duplicate slot).
 */
template <typename ElemT, typename Id_t = uint> class RegisterCollection {
  public:
    typedef ptr<ElemT> ptr_t;

  private:
    vec<ptr_t> collection{};
    vec<Id_t> emptyPositions{};

  public:
    RegisterCollection() { emptyPositions.reserve(0xfff); }

    const vec<ptr_t> getCollection() const {
        vec<ptr_t> list{};
        auto n = collection.size();
        list.reserve(n);
        for (Id_t i = 0; i < n; i++) {
            ptr_t elem = collection[i];
            if (elem)
                list.push_back(elem);
        }
        return list;
    }

    const vec<ptr_t> &getCollectionWithGaps() const { return collection; }

    ptr_t getElemAt(Id_t idx) { return collection[idx]; }

    Id_t registerPtr(ptr_t pObject) {
        if (emptyPositions.empty()) {
            collection.push_back(pObject);
            return static_cast<Id_t>(collection.size() - 1);
        }
        // Reuse the slot that was freed last: back() is a collection index,
        // unlike size()-1 which is an index into the free list itself.
        const Id_t idx = emptyPositions.back();
        emptyPositions.pop_back();
        collection[idx] = pObject;
        return idx;
    }

    ElemT &get(Id_t idx) { return *collection[idx]; }
    ptr_t getPtr(Id_t idx) { return collection[idx]; }

    Id_t registerObj(ElemT object) {
        return registerPtr(ptr_t{new ElemT(object)});
    }

    void unregister(Id_t index) {
        // A stale/duplicate id (or the sentinel a second Unregister() returns)
        // must not push a second hole for the same slot: that would make two
        // future registers hand out the same index.
        if (index >= collection.size() || !collection[index])
            return;
        collection[index] = nullptr;
        emptyPositions.push_back(index);
    }

    void cleanup() {
        collection.clear();
        emptyPositions.clear();
    }
};

NSP_UTIL_END
