/*
 * ObjectChurnTest — headless regression test for Util::RegisterCollection.
 *
 * RegisterCollection is the slot/free-list container SceneRender uses for its
 * mesh queues (one collection per material). The voxel game registers and
 * unregisters meshes continuously (chunk load/unload), so the container has to
 * survive arbitrary interleaved register/unregister cycles:
 *
 *   - a stored id must always resolve to the very same object (identity),
 *   - a freed slot must be reused exactly once by the next register,
 *   - no slot may be handed out twice,
 *   - the free list must not accumulate duplicate holes.
 *
 * Regression: registerPtr() used to write collection[emptyPositions.size()-1]
 * instead of collection[emptyPositions.back()], so after the first unregister
 * every new element overwrote slot 0 and the freed slot stayed empty - one
 * element silently vanished, another was returned for two different ids.
 *
 * No window, no graphics device: this binary must run everywhere.
 */

#include <algorithm>
#include <cstdio>
#include <random>
#include <set>
#include <vector>

#include "Rice/Util/RegisterCollection.hpp"

using namespace Rice::Util;

namespace {

uint g_failures = 0;

void check(bool condition, const char *what, uint iter = 0) {
    if (!condition) {
        std::printf("FAIL [iteration %u]: %s\n", iter, what);
        ++g_failures;
    }
}

/* Payload whose identity is observable: the collection must never swap two of
 * these pointers. */
struct Element {
    uint id = 0;
    uint tag = 0;
};

using Collection = RegisterCollection<Element, uint>;
using ElementPtr = ptr<Element>;

/* Number of non-null slots, i.e. the number of live registrations. */
uint liveCount(const Collection &c) {
    uint n = 0;
    for (auto &p : c.getCollectionWithGaps())
        if (p) ++n;
    return n;
}

/* Indices of the holes currently present in the slot vector. */
std::set<uint> holes(const Collection &c) {
    std::set<uint> r;
    auto &slots = c.getCollectionWithGaps();
    for (uint i = 0; i < slots.size(); ++i)
        if (!slots[i]) r.insert(i);
    return r;
}

/* Smallest case that used to break: 3 elements, free the first, re-register. */
bool testReuseAfterFirstUnregister() {
    Collection c;
    std::vector<ElementPtr> v;
    for (uint i = 0; i < 3; ++i)
        v.push_back(new_ptr<Element>(Element{i, 0}));

    std::vector<uint> ids;
    for (auto &p : v) ids.push_back(c.registerPtr(p));
    check(ids == std::vector<uint>{0, 1, 2}, "ids must be 0,1,2 on a fresh collection");

    // Free slot 0 (the one that used to be overwritten by the next register).
    c.unregister(ids[0]);
    check(holes(c) == std::set<uint>{0}, "slot 0 must be the only hole");

    auto fresh = new_ptr<Element>(Element{100, 0});
    uint newIdx = c.registerPtr(fresh);
    check(newIdx == 0, "the freed slot 0 must be reused");
    check(c.getPtr(0) == fresh, "slot 0 must hold the newly registered element");
    check(c.getPtr(1) == v[1] && c.getPtr(2) == v[2],
          "the untouched elements must keep their identity");
    check(holes(c).empty(), "the collection must have no holes left");
    check(liveCount(c) == 3, "expected 3 live elements");
    return g_failures == 0;
}

/* Every hole is reused exactly once, and returns to the free list on unregister. */
bool testHolesReusedExactlyOnce() {
    constexpr uint n = 64;
    Collection c;
    std::vector<ElementPtr> v;
    std::vector<uint> ids;
    for (uint i = 0; i < n; ++i) {
        v.push_back(new_ptr<Element>(Element{i, 0}));
        ids.push_back(c.registerPtr(v.back()));
    }
    check(liveCount(c) == n, "expected all elements registered");

    // Free a scattered subset (every third element).
    std::set<uint> freed;
    for (uint i = 0; i < n; i += 3) {
        c.unregister(ids[i]);
        freed.insert(ids[i]);
        v[i].reset(); // drop the owner: the collection must still be consistent
    }
    check(holes(c) == freed, "holes must match the unregistered slots");
    uint live = liveCount(c);
    check(live == n - freed.size(), "live count after unregister");

    // Re-register exactly as many elements as there are holes.
    std::vector<uint> taken;
    for (uint i = 0; i < freed.size(); ++i) {
        auto p = new_ptr<Element>(Element{1000 + i, 0});
        uint idx = c.registerPtr(p);
        taken.push_back(idx);
        check(freed.count(idx) == 1, "a register must land in a freed slot");
        check(c.getPtr(idx) == p, "the reused slot must hold the new element");
    }
    check(holes(c).empty(), "all holes must have been consumed exactly once");
    check(std::set<uint>(taken.begin(), taken.end()).size() == taken.size(),
          "no slot may be handed out twice");
    check(liveCount(c) == n, "live count after re-registering");

    // The survivors still resolve to themselves.
    for (uint i = 0; i < n; ++i) {
        if (freed.count(ids[i])) continue;
        check(c.getPtr(ids[i]) == v[i], "survivor identity lost");
    }
    return g_failures == 0;
}

/* Interleaved churn over many frames, tracking every id -> pointer mapping. */
bool testInterleavedChurn() {
    constexpr uint ops = 20000;
    constexpr uint pool = 256;

    std::mt19937 rng(20240521); // fixed seed: the check must be deterministic
    Collection c;
    std::map<uint, ElementPtr> byId;

    for (uint i = 0; i < ops; ++i) {
        bool doRegister = byId.size() < pool / 2 ? true
                          : byId.size() > pool      ? false
                                                    : (rng() & 1) == 0;
        if (doRegister) {
            auto p = new_ptr<Element>(Element{i, 0});
            uint idx = c.registerPtr(p);

            check(byId.count(idx) == 0, "register returned an id that is still live", i);
            check(c.getPtr(idx) == p, "getPtr(id) != registered pointer", i);
            check(c.getCollectionWithGaps()[idx] == p, "slot does not hold the pointer", i);
            byId[idx] = p;
        } else if (!byId.empty()) {
            auto it = byId.begin();
            std::advance(it, rng() % byId.size());
            uint idx = it->first;
            ElementPtr expected = it->second;
            check(c.getPtr(idx) == expected, "id no longer resolves to its pointer", i);
            c.unregister(idx);
            byId.erase(it);
            check(c.getCollectionWithGaps()[idx] == nullptr, "unregistered slot not nulled", i);
            // A second unregister of the same (now stale) id must be harmless.
            c.unregister(idx);
            auto &slots = c.getCollectionWithGaps();
            check(holes(c).size() ==
                      (size_t)std::count_if(slots.begin(), slots.end(),
                                            [](const ElementPtr &p) { return !p; }),
                  "duplicate hole created by a stale unregister", i);
        }

        // Invariants after every single operation.
        check(liveCount(c) == byId.size(), "live count drifted", i);
        for (auto &kv : byId)
            check(c.getPtr(kv.first) == kv.second, "element identity lost", i);
    }

    // Every hole must be reusable: fill them all and check the live count.
    auto remaining = holes(c);
    uint before = liveCount(c);
    for (size_t i = 0; i < remaining.size(); ++i) {
        auto p = new_ptr<Element>(Element{0xFFFF0000u + (uint)i, 0});
        uint idx = c.registerPtr(p);
        check(remaining.count(idx) == 1, "hole reuse after churn");
        check(c.getPtr(idx) == p, "reused slot identity after churn");
    }
    check(holes(c).empty(), "holes left after refill");
    check(liveCount(c) == before + remaining.size(), "live count after refill");
    return g_failures == 0;
}

/* A stale id must not create a second hole for the same slot: that would make
 * two later registers return the same index and one element would be lost. */
bool testStaleUnregisterCannotDoubleFree() {
    Collection c;
    auto a = new_ptr<Element>(Element{1, 0});
    auto b = new_ptr<Element>(Element{2, 0});
    uint ia = c.registerPtr(a);
    c.registerPtr(b);
    c.unregister(ia);
    c.unregister(ia); // stale: the slot is already free

    auto x1 = new_ptr<Element>(Element{10, 0});
    auto x2 = new_ptr<Element>(Element{11, 0});
    uint i1 = c.registerPtr(x1);
    uint i2 = c.registerPtr(x2);
    check(i1 != i2, "a stale unregister handed the same slot out twice");
    check(c.getPtr(i1) == x1 && c.getPtr(i2) == x2, "reused slots must keep identity");
    check(liveCount(c) == 3, "expected 3 live elements after a stale unregister");
    return g_failures == 0;
}

/* cleanup() must reset both the slots and the free list. */
bool testCleanup() {
    Collection c;
    auto a = new_ptr<Element>(Element{1, 0});
    auto b = new_ptr<Element>(Element{2, 0});
    uint ia = c.registerPtr(a);
    uint ib = c.registerPtr(b);
    c.unregister(ia);
    c.cleanup();
    check(c.getCollectionWithGaps().empty(), "cleanup must drop the slots");
    uint ic = c.registerPtr(a);
    check(ic == 0 && c.getPtr(0) == a, "cleanup must drop the free list too");
    (void)ib;
    return g_failures == 0;
}

} // namespace

int main() {
    struct Case {
        const char *name;
        bool (*fn)();
    } cases[] = {
        {"reuse after first unregister", testReuseAfterFirstUnregister},
        {"holes reused exactly once", testHolesReusedExactlyOnce},
        {"interleaved churn", testInterleavedChurn},
        {"stale unregister does not double free", testStaleUnregisterCannotDoubleFree},
        {"cleanup", testCleanup},
    };

    uint failed = 0;
    for (auto &tc : cases) {
        uint before = g_failures;
        bool ok = tc.fn();
        if (ok && g_failures == before) {
            std::printf("PASS: %s\n", tc.name);
        } else {
            std::printf("FAILED: %s\n", tc.name);
            ++failed;
        }
    }

    if (failed != 0 || g_failures != 0) {
        std::printf("ObjectChurnTest: %u case(s) failed, %u assertion(s)\n", failed, g_failures);
        return 1;
    }
    std::printf("ObjectChurnTest: all cases passed\n");
    return 0;
}
