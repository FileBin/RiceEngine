#include "Rice/Scene/Component.hpp"
#include "component.hpp"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>

#include "ComponentSerializer.hpp"

extern "C" {

void *createComponent() {
    p_component c;
    c = new component;
    return static_cast<Rice::Components::Component *>(c);
}

void destroyComponent(void *p_component) {
    component *c =
        dynamic_cast<component *>((Rice::Components::Component *)p_component);
    delete c;
}

byte *serializeComponent(void *p_component) {
    component *c =
        dynamic_cast<component *>((Rice::Components::Component *)p_component);
    assert(c != nullptr);
    auto data = serializator<component *>().to_bytes(c);
    size_t n = strlen((const char *)data.data());
    byte *arr = (byte *)calloc(n, 1);
    memcpy((void *)arr, (void *)data.data(), n);
    return arr;
}

size_t deserializeComponent(byte *arr, void *&p_component) {
    component *c =
        dynamic_cast<component *>((Rice::Components::Component *)p_component);
    data_t data;
    memcpy((void *)data.data(), (void *)arr, strlen((char *)arr));
    size_t n = serializator<component *>().from_bytes(data, c);
    p_component = static_cast<Rice::Components::Component *>(c);
    return n;
}

int64_t getId() { return std::hash<std::string>()(std::string(COMPONENT_STR)); }
const char *getName() { return COMPONENT_STR; }
}

#undef PFX