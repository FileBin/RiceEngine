#pragma once

#define ZERO_MEMORY(ptr, n) memset((void *)ptr, 0, n);
#define DESTROY(p)                                                                            \
    if (p) {                                                                                  \
        delete (p);                                                                           \
        (p) = nullptr;                                                                        \
    }
#define DESTROY_ARRAY(p)                                                                      \
    if (p) {                                                                                  \
        delete[](p);                                                                          \
        (p) = nullptr;                                                                        \
    }

#define sign(f) (signbit(f) ? 1 : -1)
