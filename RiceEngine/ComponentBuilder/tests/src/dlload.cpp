#include <cassert>
#include <dlfcn.h>

#include "Rice/Engine.hpp"
#include "Rice/Scene/Component.hpp"

typedef void *(*component_create_func)();
typedef void (*component_destroy_func)(void *);

int main() {
    constexpr auto so_path =
        "/home/filebin/GitRepo/RiceEngine/dev/build/Debug/RiceEngine/"
        "ComponentBuilder/tests/Components/TestComponent/libTestComponent.so";
    void *lib_handle = dlopen(so_path, RTLD_NOW);
    if (!lib_handle) {
        fprintf(stderr, "dlopen() error: %s\n", dlerror());
        exit(1);
    }
    assert(lib_handle);
    component_create_func createComponent =
        (component_create_func)dlsym(lib_handle, "createComponent");
    component_destroy_func destroyComponent =
        (component_destroy_func)dlsym(lib_handle, "destrioyComponent");

    Rice::Components::Component *c =
        (Rice::Components::Component *)createComponent();
    bool b = c->isEnabled();
    destroyComponent((void *)c);
    dlclose(lib_handle);
    return 0;
}