#include <cassert>
#include <dlfcn.h>
#include <iostream>
#include <memory>

#include "Rice/Engine.hpp"
#include "Rice/Scene/Component.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

typedef Rice::Components::Component *(*component_create_func)();
typedef void (*component_destroy_func)(Rice::Components::Component *);
typedef rapidjson::Value (*component_serialize_to_json_func)(
    void *, rapidjson::Document::AllocatorType);

[[nodiscard]] void *load_lib(const char *path) {
    void *lib_handle = dlopen(path, RTLD_NOW);
    if (!lib_handle) {
        fprintf(stderr, "dlopen() error: %s\n", dlerror());
        return 0;
    }
    return lib_handle;
}

bool test1() {
    constexpr auto so_path = "./Components/TestComponent/libTestComponent.so";
    void *lib_handle = load_lib(so_path);
    assert(lib_handle);
    component_create_func createComponent =
        (component_create_func)dlsym(lib_handle, "createComponent");
    component_destroy_func destroyComponent =
        (component_destroy_func)dlsym(lib_handle, "destroyComponent");

    component_serialize_to_json_func serializeJson =
        (component_serialize_to_json_func)dlsym(lib_handle,
                                                "serializeComponentJson");

    Rice::Components::Component *component = createComponent();

    bool b = component->isEnabled();

    rapidjson::Document d;

    auto v = serializeJson(component, d.GetAllocator());

    rapidjson::StringBuffer buffer;

    buffer.Clear();

    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    v.Accept(writer);

    std::cout << buffer.GetString() << "\n";
    destroyComponent(component);
    dlclose(lib_handle);
    return true;
}

bool test2() {
    constexpr auto so_path1 = "./Components/TestComponent/libTestComponent.so";
    constexpr auto so_path2 = "./Components/Camera/libCamera.so";
    void *lib_handle1 = load_lib(so_path1);
    assert(lib_handle1);
    void *lib_handle2 = load_lib(so_path2);
    assert(lib_handle2);

    component_create_func createComponent1 =
        (component_create_func)dlsym(lib_handle1, "createComponent");

    component_create_func createComponent2 =
        (component_create_func)dlsym(lib_handle2, "createComponent");
    return createComponent1 != createComponent2;
}

typedef bool (*test_func)();

int main() {
    test_func tests[] = {test1, test2};
    constexpr auto n = sizeof(tests) / sizeof(test_func);
    for (uint i = 0; i < n; i++) {
        if (tests[i]()) {
            std::cout << "Test " << i << " passed!\n";
        } else {
            std::cout << "Test " << i << " failed!\n";
        }
    }
}