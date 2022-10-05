#include <bits/stdc++.h>
#include <iostream>
#include <ostream>

#include "../res/component.hpp"
#include "pod_reflection.hpp"

class sus {
  public:
    int a;
    bool b;
    Rice::Components::TestCamera cam;
};

int main() {
    using namespace Rice::Components;
    std::cout << eld::pod_size<sus>::value << std::endl;
    std::cout
        << typeid(eld::pod_to_tuple_t<sus, eld::extend_feed<TestCamera>>).name()
        << std::endl;
}