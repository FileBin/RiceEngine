#include "Rice/defines.h"
#include <algorithm>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <typeindex>
#include <vector>

namespace Meta {
typedef std::map<unsigned long, void *> TypeMap;
}

template <typename T> struct Member;

enum class Types {
    Struct,
    BuiltIn,
};

template <typename T, typename... MembersT> struct Type {
    Types type;
    std::string name;
    std::tuple<MembersT T::*...> members;

  private:
    Type() = default;
    Type(const Type &) = default;
    Type(Type &&) = default;

    Type(Types type, std::string name, MembersT T::*...members) : type(type), name(name), members(std::make_tuple(members...)) {}
    friend void BuildTypes(Meta::TypeMap &map);
    friend void BuildPrimitiveTypes(Meta::TypeMap &map);
};

template <typename T> struct Type<T> {
    Types type;
    std::string name;

  private:
    Type() = default;
    Type(const Type &) = default;
    Type(Type &&) = default;
    Type(Types type, std::string name) : type(type), name(name) {}

    friend void BuildTypes(Meta::TypeMap &map);
    friend void BuildPrimitiveTypes(Meta::TypeMap &map);
};

void BuildTypes(Meta::TypeMap &map);

namespace Meta {
struct TypeBuilder {
    TypeMap type_map;
    TypeBuilder(void (*func)(TypeMap &map)) { func(type_map); }
};

static TypeBuilder builder = BuildTypes;
} // namespace Meta

inline void BuildPrimitiveTypes(Meta::TypeMap &map) {
#define BUILTIN_GEN_TYPE(b) map.insert({typeid(b).hash_code(), (void *)new Type<b>{Types::BuiltIn, #b}})
    BUILTIN_GEN_TYPE(bool);
    BUILTIN_GEN_TYPE(char);
    BUILTIN_GEN_TYPE(unsigned char);
    BUILTIN_GEN_TYPE(short);
    BUILTIN_GEN_TYPE(unsigned short);
    BUILTIN_GEN_TYPE(int);
    BUILTIN_GEN_TYPE(unsigned int);
    BUILTIN_GEN_TYPE(long);
    BUILTIN_GEN_TYPE(unsigned long);
    BUILTIN_GEN_TYPE(float);
    BUILTIN_GEN_TYPE(double);
    BUILTIN_GEN_TYPE(long double);
    BUILTIN_GEN_TYPE(wchar_t);
#undef BUILTIN_GEN_TYPE
}

/* example

struct Foo {
    int i;
    char ch;
};

struct Bar {
    Foo foo;
    double dbl;
};

inline void BuildTypes(Meta::TypeMap &map) {
    map.insert({typeid(Foo).hash_code(), (void *)new Type<Foo, int, char>{Types::Struct, "Foo", &Foo::i, &Foo::ch}});
    map.insert({typeid(Bar).hash_code(), (void *)new Type<Bar, Foo, double>{Types::Struct, "Bar", &Bar::foo, &Bar::dbl}});
}
*/