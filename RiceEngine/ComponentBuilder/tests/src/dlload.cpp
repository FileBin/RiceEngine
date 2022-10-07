#include <cassert>
#include <dlfcn.h>
#include <iostream>

#include "Rice/Engine.hpp"
#include "Rice/Scene/Component.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

typedef void *(*component_create_func)();
typedef void (*component_destroy_func)(void *);
typedef rapidjson::Value (*component_serialize_to_json_func)(
    void *, rapidjson::Document::AllocatorType);

int main() {
    constexpr auto so_path = "./Components/TestComponent/libTestComponent.so";
    void *lib_handle = dlopen(so_path, RTLD_NOW);
    if (!lib_handle) {
        fprintf(stderr, "dlopen() error: %s\n", dlerror());
        exit(1);
    }
    assert(lib_handle);
    component_create_func createComponent =
        (component_create_func)dlsym(lib_handle, "createComponent");
    component_destroy_func destroyComponent =
        (component_destroy_func)dlsym(lib_handle, "destroyComponent");

    component_serialize_to_json_func serializeJson =
        (component_serialize_to_json_func)dlsym(lib_handle,
                                                "serializeComponentJson");

    Rice::Components::Component *c =
        (Rice::Components::Component *)createComponent();
    bool b = c->isEnabled();

    rapidjson::Document d;

    auto v = serializeJson(c, d.GetAllocator());

    rapidjson::StringBuffer buffer;

    buffer.Clear();

    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    v.Accept(writer);

    std::cout << buffer.GetString() << "\n";

    destroyComponent((void *)c);
    dlclose(lib_handle);
    return 0;
}