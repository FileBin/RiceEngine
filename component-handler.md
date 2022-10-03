# Component Handler

## 1. Add class ComponentHandler to core

* ### Description

  ComponentHandler is dynamic library wrapper class used for load code of component into core. In class construcor it will create library handle path (path will be provided without extention for platform compability).

  ```c++
  #include <dlfcn.h>
    
  void* lib_handle = dlopen(
      "/home/mike/Desktop/TargetProcess/MyLib.so.1.0.1",
      RTLD_NOW|RTLD_GLOBAL);
  ```

  After succefully loading the library it sets function pointers from library.

  ```c++
  void (*some_func)() = dlsym(lib_handle, "someFunc");
  ```

  In destructor it will unload dynamic library

  ```c++
  dlclose(lib_handle);
  ```

  > NOTE: all \<dlfcn.h\> functions will be wrapped in namespace Util

* ### UML diagram
  
  ```mermaid
  classDiagram
  class ComponentHandler {
    -void* lib_hanble
    -p_comp_constructor constructor_func
    -p_comp_descrtuctor destructor_func
    -p_comp_serialize serialize_func
    -p_comp_deserialize deserialize_func
    -ComponentHandler(String lib_path)
    +~ComponentHandler()
    +createComponent() ptr~ComponentBase~
    +serializeComponent(ptr~Component~ comp) vec~byte~ 
    +deserializeComponent(vec~byte~) ptr~Component~  
  }
  ```

## 2. Add support of dynamic component loading

* Add method loadComponent(String) to Engine
* Add loading of default components (like Mesh Camera) in core initialization
