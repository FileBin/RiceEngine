#include <algorithm>
#include <bits/utility.h>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <typeindex>
#include <vector>

#include "nameof/nameof.hpp"

namespace Meta {
typedef size_t TypeHash;
typedef std::map<TypeHash, void *> TypeMap;

enum class Types {
    Struct,
    BuiltIn,
};

template <typename T, typename... MembersT> class Type;

template <typename T> class Member {
  private:
    template <typename R, typename... MembersT> friend class Type;
    T member_pointer;

    Member(T pointer) : member_pointer(pointer) {}

  public:
    constexpr char *getName() { return NAMEOF_MEMBER(member_pointer); }
    constexpr T getMemberPointer() { return member_pointer; }
};

template <typename T, typename... MembersT> class Type {
  private:
    Types type;
    typedef std::tuple<MembersT T::*...> tuple_t;
    tuple_t members;
    Type() = default;
    Type(const Type &) = default;
    Type(Type &&) = default;

    Type(Types type, MembersT T::*...members) : type(type), members(std::make_tuple(members...)) {}
    friend void BuildTypes(TypeMap &map);
    friend void BuildPrimitiveTypes(TypeMap &map);

  public:
    constexpr char *getFullName() { return NAMEOF_TYPE(T); }
    constexpr char *getShortName() { return NAMEOF_SHORT_TYPE(T); }
    constexpr tuple_t &getMembers() { return members; }
    constexpr size_t getMembersCount() { return std::tuple_size_v<tuple_t>; }
    template <size_t i> constexpr auto getMemberAt() { return Member(std::get<i>(members)); }
};

template <typename T> class Type<T> {
    Types type;
    Type() = default;
    Type(const Type &) = default;
    Type(Type &&) = default;
    Type(Types type) : type(type) {}

    friend void BuildTypes(Meta::TypeMap &map);
    friend void BuildPrimitiveTypes(Meta::TypeMap &map);

  public:
    constexpr char *getFullName() { return NAMEOF_TYPE(T); }
    constexpr char *getShortName() { return NAMEOF_SHORT_TYPE(T); }
    constexpr size_t getMembersCount() { return 0; }
};

void BuildTypes(Meta::TypeMap &map);

class TypeBuilder {
    friend const auto &type_of();
    TypeMap type_map;

  public:
    TypeBuilder(void (*func)(TypeMap &map)) { func(type_map); }
};

static TypeBuilder builder = BuildTypes;

template <typename T> const auto &type_of() { return builder.type_map.at(typeid(T).hash_code()); }

inline void BuildPrimitiveTypes(TypeMap &map) {
#define BUILTIN_GEN_TYPE(b) map.insert({typeid(b).hash_code(), (void *)new Type<b>{Types::BuiltIn}})
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

} // namespace Meta

/* example

struct Foo {
    int i;
    char ch;
};

struct Bar {
    Foo foo;
    double dbl;
};
inline void Meta::BuildTypes(TypeMap &map) {
    map.insert({typeid(Foo).hash_code(), (void *)new Type<Foo, int, char>{Types::Struct, &Foo::i, &Foo::ch}});
    map.insert({typeid(Bar).hash_code(), (void *)new Type<Bar, Foo, double>{Types::Struct, &Bar::foo, &Bar::dbl}});
}
*/