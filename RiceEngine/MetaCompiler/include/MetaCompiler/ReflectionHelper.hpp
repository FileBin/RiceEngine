#include <algorithm>
#include <bits/utility.h>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <typeindex>
#include <vector>

#include "nameof/nameof.hpp"

namespace Meta {

template <size_t Index = 0,                                                 // start iteration at 0 index
          typename TTuple,                                                  // the tuple type
          size_t Size = std::tuple_size_v<std::remove_reference_t<TTuple>>, // tuple size
          typename TCallable, // the callable to bo invoked for each tuple item
          typename... TArgs   // other arguments to be passed to the callable
          >
void for_each(TTuple &&tuple, TCallable &&callable, TArgs &&...args) {
    if constexpr (Index < Size) {
        if constexpr (std::is_assignable_v<
                          bool &, std::invoke_result_t<TCallable &&, TArgs &&..., decltype(std::get<Index>(tuple))>>) {
            if (!std::invoke(callable, args..., std::get<Index>(tuple)))
                return;
        } else {
            std::invoke(callable, args..., std::get<Index>(tuple));
        }

        if constexpr (Index + 1 < Size)
            for_each<Index + 1>(std::forward<TTuple>(tuple), std::forward<TCallable>(callable),
                                std::forward<TArgs>(args)...);
    }
}

typedef size_t TypeHash;
typedef std::map<TypeHash, void *> TypeMap;

template <typename T> struct TypeOf;

enum class Types {
    Struct,
    BuiltIn,
};

template <typename T, typename... MembersT> class Type;

template <typename T> class Member {
    template <typename R> friend struct TypeOf;
    template <typename R, typename... MembersT> friend class Type;

  private:
    std::string name;
    T member_pointer;

    Member(std::string name, T pointer) : name(name), member_pointer(pointer) {}

  public:
    std::string getName() const { return name; }
    constexpr T getMemberPointer() { return member_pointer; }
};

template <typename T, typename... MembersT> class Type {
    template <typename R> friend struct TypeOf;

  private:
    Types type;
    typedef std::tuple<Member<MembersT T::*>...> tuple_t;
    tuple_t members;
    Type(Type &&) = default;

    Type(Types type, Member<MembersT T::*>... members) : type(type), members(std::make_tuple(members...)) {}

  public:
    constexpr char *getFullName() { return NAMEOF_TYPE(T); }
    constexpr char *getShortName() { return NAMEOF_SHORT_TYPE(T); }
    tuple_t getMembers() { return members; }
    constexpr size_t getMembersCount() { return std::tuple_size_v<tuple_t>; }
    template <size_t i> constexpr decltype(auto) getMemberAt() { return std::get<i>(members); }
};

template <typename T> class Type<T> {
    template <typename R> friend struct TypeOf;
    Types type;
    Type() = default;
    Type(Types type) : type(type) {}

  public:
    constexpr char *getFullName() { return NAMEOF_TYPE(T); }
    constexpr char *getShortName() { return NAMEOF_SHORT_TYPE(T); }
    constexpr size_t getMembersCount() { return 0; }
};

#define BUILTIN_GEN_TYPE(b)                                                                                            \
    template <> struct TypeOf<b> {                                                                                     \
        Type<b> type() { return Type<b>{Types::BuiltIn}; }                                                             \
    }

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

} // namespace Meta

#define REFLECTABLE __attribute__((annotate("reflectable")))

/*example*/

struct Foo {
    int i;
    char ch;
};

struct Bar {
    Foo foo;
    double dbl;
};

template <> struct Meta::TypeOf<Foo> {
    Type<Foo, int, char> type() { return Type<Foo, int, char>{Types::Struct, {"i", &Foo::i}, {"ch", &Foo::ch}}; }
};

template <> struct Meta::TypeOf<Bar> {
    Type<Bar, Foo, double> type() {
        return Type<Bar, Foo, double>{Types::Struct, {"foo", &Bar::foo}, {"dbl", &Bar::dbl}};
    }
};

int main() {
    auto type = Meta::TypeOf<Foo>().type();
    auto members = type.getMembers();
    Meta::for_each(members, [](const auto &member) { std::cout << member.getName() << "\n"; });
    return 0;
}