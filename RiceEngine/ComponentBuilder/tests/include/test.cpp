#define REFLECTABLE __attribute__((annotate("reflectable")))
#define NOT_REFLECTABLE __attribute__((annotate("not_reflectable")))
#define TEST __attribute__((annotate("test")))

#include <string>

struct REFLECTABLE struct111 {
    TEST TEST TEST TEST int i;
};

struct REFLECTABLE struct222 {
    REFLECTABLE double d;
    struct REFLECTABLE struc333 {
        REFLECTABLE std::string i;
    };
};
