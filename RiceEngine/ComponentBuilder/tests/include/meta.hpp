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
        Struct,
        BuiltIn,
    } type;
    std::string name;
    std::unique_ptr<Type> base;
    std::vector<std::unique_ptr<Field>> fields;

    Type(Enum type, std::string name) : type(type), name(name) {}
    Type(Enum type, std::string name, std::vector<Field> vec);
};

struct Field {
    Type type;
    std::string name;
};

inline Type::Type(Enum type, std::string name, std::vector<Field> vec) : type(type), name(name) {
    //.. vector stuff
}

namespace Meta {
template <typename T> struct TypeOf;
}

// autogenerated

namespace Meta {
template <> struct TypeOf<Name> {
    static Type type() {
        Type type(Type::Struct, "Name");

        std::unique_ptr<Field> field;

        field.reset(new Field{{Type::BuiltIn, "int"}, "i"});
        type.fields.push_back(std::move(field));

        field.reset(new Field{{Type::BuiltIn, "char"}, "ch"});
        type.fields.push_back(std::move(field));

        field.reset(new Field{{Type::Struct, "std::string"}, "str"});
        type.fields.push_back(std::move(field));

        return type;
    }
};
} // namespace Meta

namespace Meta {
template <> struct TypeOf<Testsssss> {
    static Type type() {
        Type type(Type::Struct, "Testsssss");
        std::unique_ptr<Field> fieldfield.reset(new Field{{Type::BuiltIn, "int"}, "i"});
        type.fields.push_back(std::move(field));
        field.reset(new Field{{Type::Struct, "std::basic_string<char>"}, "g"});
        type.fields.push_back(std::move(field));
        return type;
    }
};
template <> struct TypeOf<SUS> {
    static Type type() {
        Type type(Type::Struct, "SUS");
        std::unique_ptr<Field> fieldfield.reset(new Field{{Type::BuiltIn, "short"}, "kk"});
        type.fields.push_back(std::move(field));
        return type;
    }
};
template <> struct TypeOf<SUS::mememe> {
    static Type type() {
        Type type(Type::Struct, "SUS::mememe");
        std::unique_ptr<Field> fieldfield.reset(new Field{{Type::BuiltIn, "int"}, "inner_int"});
        type.fields.push_back(std::move(field));
        return type;
    }
};
} // namespace Meta