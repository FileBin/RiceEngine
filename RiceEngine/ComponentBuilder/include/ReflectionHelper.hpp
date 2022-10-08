#include <algorithm>
#include <memory>
#include <string>
#include <vector>
// header
class Name {
    int i;
    char ch;
    std::string str;
};

struct Field;

struct Type {
    enum Enum {
        Class,
        Struct,
        BuiltIn,
    } type;
    std::string name;
    std::unique_ptr<Type> base;
    std::vector<std::unique_ptr<Field>> fields;

    Type(Enum type, std::string name) : type(type), name(name) {}
};

struct Field {
    Type type;
    std::string name;
};

namespace Meta {
template <typename T> struct TypeOf;
}