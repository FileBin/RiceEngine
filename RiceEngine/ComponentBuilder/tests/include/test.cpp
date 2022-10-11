#define REFLECTABLE __attribute__((annotate("reflectable")))

#include <string>

struct REFLECTABLE struct111 {
    int i;
};

struct REFLECTABLE struct222 {
    double d;
    struct REFLECTABLE struc333 {
        std::string i;
    };
};
