
#include <cstdint>
const std::set<std::string> builtins = {"unsigned", "long", "const", "static", "constexpr", "char", "wchar_t", "short", "int", "long", "double"};
const std::set<std::string> ignored_names = {"__locale_struct"};

std::vector<std::string> split(const std::string &target, char c) {
    std::string temp;
    std::stringstream ss{target};
    std::vector<std::string> result;

    while (std::getline(ss, temp, c)) {
        result.push_back(temp);
    }

    return result;
}

bool isBuiltIn(std::string str) {
    if (str.at(str.size() - 1) == ']') {
        str = str.substr(0, str.find('['));
    }

    if (str.at(str.size() - 1) == '*') {
        return true;
    }

    std::vector<std::string> tokens = split(str, ' ');

    for (const auto &token : tokens) {
        if (!builtins.contains(token)) {
            return false;
        }
    }

    return true;
}

std::string parseStruct(const nlohmann::json::value_type &item, const std::string current_path) {
    using json = nlohmann::json;
    using namespace std;

    string built_component;
    string inner_componenets;

    if (item.contains("completeDefinition") && item.contains("name")) {

        string name = item["name"];

        if (ignored_names.contains(name)) {
            return "";
        }

        string type = item["tagUsed"];

        if (type == "struct" || type == "class") {

            if (current_path.size()) {
                name = current_path + "::" + name;
            }

            built_component += "template <> struct TypeOf<" + name +
                               "> {\n"
                               "static Type type() {\n";

            built_component += "Type type(Type::Struct, \"" + name + "\");\n" + "std::unique_ptr<Field> field";

            for (const auto &[field_index, field] : item["inner"].items()) {
                if (field["kind"] == "FieldDecl") {

                    string fname = field["name"];
                    auto ftype_raw = field["type"];
                    string ftype = ftype_raw.contains("desugaredQualType") ? ftype_raw["desugaredQualType"] : ftype_raw["qualType"];

                    built_component += "field.reset(new Field{{Type::";

                    if (isBuiltIn(ftype)) {
                        built_component += "BuiltIn";
                    } else {
                        built_component += "Struct";
                    }

                    built_component += ", \"" + ftype + "\"}, \"" + fname + "\"});\n";

                    built_component += "type.fields.push_back(std::move(field));\n";
                } else if (field["kind"] == "CXXRecordDecl" && field.contains("completeDefinition")) {
                    inner_componenets += parseStruct(field, name);
                }
            }
            built_component += "return type;}};";
        }
    }

    if (built_component.size() || inner_componenets.size()) {
        return "\n" + built_component + inner_componenets;
    }
    return "";
}

int main() {
    using json = nlohmann::json;
    using namespace std;

    auto start = chrono::steady_clock::now();

    std::ifstream f("./res/ast.json");
    json data = json::parse(f);

    string built_component = "namespace Meta {";

    for (const auto &[item_index, item] : data["inner"].items()) {
        built_component += parseStruct(item, "");
    }

    built_component += "}\n";

    std::cout << built_component;

    cout << "Built in: " << chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() << " sec";

    return 0;
}