
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

const std::set<std::string> builtins = {"unsigned", "long",  "const", "static", "constexpr", "char",
                                        "wchar_t",  "short", "int",   "long",   "double"};
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

typedef std::vector<std::string> Location;

struct Field {
    std::string name;
    std::string type;
    friend std::ostream &operator<<(std::ostream &os, const Field &field);
};

std::ostream &operator<<(std::ostream &os, const Field &field) {
    os << field.type << " " << field.name << ";";
    return os;
}

struct Struct {
    Location location;
    std::string name;
    std::vector<Field> fields;
    std::string getFullName() const {
        std::string full_name;
        for (auto loc : location) {
            full_name += loc + "::";
        }
        return full_name + name;
    }
    bool is_reflectable;
    friend std::ostream &operator<<(std::ostream &os, const Struct &str);
};

std::ostream &operator<<(std::ostream &os, const Struct &str) {
    os << str.getFullName();
    os << " {";
    for (const auto &field : str.fields) {
        os << "\n    " << field;
    }
    os << "\n}";
    return os;
}

class Parser {
    Location current_location;
    std::vector<Struct> current_struct;
    std::vector<Struct> all_structs;
    std::stringstream &ss;
    int currentLevel = 0;

  public:
    Parser(std::stringstream &ss) : ss(ss) {}

    int getLineLevel() {
        using namespace std;
        int lvl = 0;
        char ch;
        stringstream::pos_type pos;
        while (true) {
            pos = ss.tellg();
            ch = ss.get();
            if (ch == '|' || ch == '`' || ch == ' ') {
                lvl++;
                continue;
            }
            break;
        }
        ss.seekg(pos);
        return lvl / 2 + 1;
    }

    void skipAllChars(char ch) {
        std::stringstream::pos_type pos;
        do {
            pos = ss.tellg();
        } while (ss.get() == ch);
        ss.seekg(pos);
    }

    void skipUntil(char ch) {
        char curr_ch;
        while (ss.peek() != ch && ss.peek() != -1) {
            curr_ch = ss.get();
        }
        ss.get();
    }

    std::vector<std::string> extractArgs() {
        std::vector<std::string> args;
        std::string args_raw;
        while (ss.peek() != '\n') {
            args_raw += ss.get();
        }
        args = split(args_raw, ' ');
        return args;
    }

    bool checkEqual(const std::string &str) {
        auto pos = ss.tellg();
        for (char ch : str) {
            if (ch != ss.get()) {
                ss.seekg(pos);
                return false;
            }
        }
        ss.seekg(pos);
        return true;
    }

    bool tryFind(const std::string &str, char end = '\n') {
        char ch;
        auto str_pos = str.begin();
        auto pos = ss.tellg();
        do {
            ch = ss.get();
            if (ch == *str_pos) {
                str_pos++;
                if (str_pos == str.end()) {
                    ss.seekg(pos);
                    return true;
                }
                continue;
            }
            str_pos = str.begin();
        } while (ch != end);
        ss.seekg(pos);
        return false;
    }

    void parseLine(bool &is_struct_definition, std::string &location) {
        auto pos = ss.tellg();
        skipAllChars('-');
        if (checkEqual("CXXRecordDecl")) {
            if (!tryFind("implicit") && (tryFind("struct") || tryFind("class"))) {
                std::vector<std::string> args = extractArgs();
                if (args.back() == "definition") {
                    args.pop_back();
                    if (args.back() != "struct" && args.back() != "class") {
                        current_struct.push_back({current_location, args.back()});
                        location = args.back();
                        is_struct_definition = true;
                    }
                }
            }
        } else if (checkEqual("FieldDecl")) {
            std::vector<std::string> args = extractArgs();
            if (!current_struct.empty()) {
                std::string type = args.back();
                type.pop_back();
                int pos = type.find_last_of('\'');
                if (pos != std::string::npos) {
                    type = type.substr(pos + 1);
                }

                current_struct.back().fields.push_back({args.at(args.size() - 2), type});
            }
        } else if (checkEqual("AnnotateAttr")) {
            if (tryFind("\"reflectable\"")) {
                current_struct.back().is_reflectable = true;
            }
        } else if (checkEqual("NamespaceDecl")) {
            std::vector<std::string> args = extractArgs();
            if (args.back() == "inline") {
                args.pop_back();
            }
            location = args.back();
        }
        skipUntil('\n');
    }

    void parseLevel(int targetLevel = 1) {
        do {
            // parse each line
            auto line_begin = ss.tellg();
            currentLevel = getLineLevel();

            if (currentLevel == targetLevel) {
                bool is_struct_definition = false;
                std::string last_location;
                parseLine(is_struct_definition, last_location);
                if (!last_location.empty()) {
                    current_location.push_back(last_location);
                    parseLevel(targetLevel + 1);
                    current_location.pop_back();
                }
                if (is_struct_definition) {
                    if (current_struct.back().is_reflectable) {
                        all_structs.push_back(current_struct.back());
                    }
                    current_struct.pop_back();
                }
            } else {
                ss.seekg(line_begin);
                if (currentLevel > targetLevel) {
                    parseLevel(targetLevel + 1);
                } else {
                    break;
                }
            }

        } while (ss.peek() != -1);
    }

    void dump() {
        for (auto &s : all_structs) {
            std::cout << s << std::endl;
        }
    }

    std::string generateMetaCode() {
        std::stringstream generated_code;
        std::string type_string;
        std::string field_string;
        std::string full_name;
        for (auto &str : all_structs) {
            field_string.clear();
            full_name = str.getFullName();
            generated_code << "template <> struct Meta::TypeOf<" + full_name;
            generated_code << "> {\n";
            type_string = "Type<" + full_name;
            for (auto &field : str.fields) {
                type_string += ", " + field.type;
                field_string += ", \n    {\"" + field.name + "\", &" + full_name + "::" + field.name + "}";
            }
            type_string += ">";
            generated_code << "    " << type_string << " type() { \n    return " << type_string << "{Types::Struct"
                           << field_string << "}; }\n};\n";
        }
        return generated_code.str();
    }
};

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

    if (!sourceFile.length() && compileCommandsFile.length()) {
        cout << "\n\nPlease set source_file_path\n";
        exit(1);
    }

    cout << "\nRunning on " << filesystem::absolute(headerFile) << "\n\n";

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

    auto start_clang = chrono::steady_clock::now();
    stringstream ss(exec("clang++" + additional_params +
                         " -Xclang -ast-dump -fsyntax-only -fno-color-diagnostics -Wno-visibility '" + headerFile +
                         "'"));
    auto end_clang = chrono::steady_clock::now();

    ofstream ast_file(headerFileName + "_ast");
    ast_file << ss.str();
    ast_file.close();

    auto start_parse = chrono::steady_clock::now();

    while (ss.get() != '\n') {
    };
    Parser parser(ss);
    parser.parseLevel();

    if (print_to_console) {
        parser.dump();
    }

    ofstream meta(headerFileName + "_meta.hpp");
    meta << parser.generateMetaCode();
    meta.close();

    cout << "\nBuilt in: "
         << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_parse).count() << "ms + "
         << chrono::duration_cast<chrono::milliseconds>(end_clang - start_clang).count() << "ms clang ast generation\n";

    return 0;
}