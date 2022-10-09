
#include <cstdint>

const std::set<std::string> builtins = {"unsigned", "long", "const", "static", "constexpr", "char", "wchar_t", "short", "int", "long", "double"};
const std::set<std::string> ignored_names = {"__locale_struct"};

std::string exec(const std::string &cmd) {
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
        return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

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

            built_component += "map.insert({typeid(" + name + ").hash_code(), (void *)new Type<" + name;

            string constructor_string = ">{Types::Struct, \"" + name + "\"";

            for (const auto &[field_index, field] : item["inner"].items()) {
                if (field["kind"] == "FieldDecl") {

                    string fname = field["name"];
                    auto ftype_raw = field["type"];
                    string ftype = ftype_raw.contains("desugaredQualType") ? ftype_raw["desugaredQualType"] : ftype_raw["qualType"];

                    built_component += ", " + ftype;

                    constructor_string += ", &" + name + "::" + fname;

                } else if (field["kind"] == "CXXRecordDecl" && field.contains("completeDefinition")) {
                    inner_componenets += parseStruct(field, name);
                }
            }

            built_component += constructor_string + "}});";
        }
    }

    if (built_component.size() || inner_componenets.size()) {
        return "\n" + built_component + inner_componenets;
    }
    return "";
}

int main(int argc, char *argv[]) {
    using json = nlohmann::json;
    using namespace std;

    if (system("clang++ -v") == -1) {
        cout << "No clang++ found, exiting\n";
        exit(1);
    }

    string compileCommandsFile;

    string sourceFile;
    string sourceFileAbsPath;

    string headerFile;
    string headerFileName;

    string curr_arg;

    bool print_to_console = false;
    for (int i = 0; i < argc; i++) {
        curr_arg = std::string(argv[i]);
        if (curr_arg.starts_with("header_file_path=")) {
            headerFile = curr_arg.substr(17);
            filesystem::path headerFilePath = headerFile;
            headerFileName = headerFilePath.stem().string();
        } else if (curr_arg.starts_with("source_file_path=")) {
            sourceFile = curr_arg.substr(17);
            filesystem::path sourceFilePath = sourceFile;
            sourceFileAbsPath = filesystem::absolute(sourceFilePath);
        } else if (curr_arg.starts_with("compile_commands_path=")) {
            compileCommandsFile = curr_arg.substr(22);
        } else if (curr_arg == "-p") {
            print_to_console = true;
        }
    }

    if (!headerFile.length()) {
        cout << "\n\nPlease set header_file_path\n";
        exit(1);
    }

    if (!sourceFile.length()) {
        cout << "\n\nPlease set source_file_path\n";
        exit(1);
    }

    cout << "Running on " << sourceFileAbsPath << "\n";

    string additional_params;

    if (compileCommandsFile.length()) {
        ifstream compileCommandsStream(compileCommandsFile);
        json compileCommandsJson = json::parse(compileCommandsStream);

        cout << "Including additonal params:";

        for (const auto &[index, compile_command] : compileCommandsJson.items()) {
            if (filesystem::absolute(compile_command["file"]) == sourceFileAbsPath) {
                vector<string> params = split(compile_command["command"], ' ');
                int index = 0;
                for (const auto &param : params) {
                    if (param == "-o") {
                        break;
                    }
                    if (index >= 1) {
                        additional_params += " " + param;
                        cout << param << "\n";
                    }
                    index++;
                }
                break;
            }
        }
    }

    const std::string ast = exec("clang++" + additional_params + " -Xclang -ast-dump -fsyntax-only -fno-color-diagnostics -Wno-visibility '" + headerFile + "'");

    auto start = chrono::steady_clock::now();

    json data = json::parse(ast);

    string built_component = "inline void BuildTypes(Meta::TypeMap &map) {";

    for (const auto &[item_index, item] : data["inner"].items()) {
        built_component += parseStruct(item, "");
    }

    built_component += "}\n";

    ofstream myfile(headerFileName + "_meta.hpp");
    myfile << built_component;
    myfile.close();

    if (print_to_console) {
        cout << built_component;
    }

    cout << "\nBuilt in: " << chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() << " sec\n";

    return 0;
}