
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

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

struct Field {
    std::string name;
    std::string type;
    friend std::ostream &operator<<(std::ostream &os, const Field &field);
};

std::ostream &operator<<(std::ostream &os, const Field &field) {
    os << "Field: " << field.name << " (type: " << field.type << ")";
    return os;
}

struct Location {
    std::vector<std::string> vec;
    friend std::ostream &operator<<(std::ostream &os, const Location &loc);
};

std::ostream &operator<<(std::ostream &os, const Location &loc) {
    os << "Location:";
    for (auto loc : loc.vec) {
        os << " " << loc;
    }
    return os;
}

struct Struct {
    Location location;
    std::string name;
    std::vector<Field> fields;
    bool is_reflectable;
    friend std::ostream &operator<<(std::ostream &os, const Struct &str);
};

std::ostream &operator<<(std::ostream &os, const Struct &str) {
    os << "Struct " << str.name << ": { " << str.location << ", { ";
    for (const auto &field : str.fields) {
        os << field << ", ";
    }
    os << " } }";
    return os;
}

class Parser {
    Location current_location;
    std::vector<Struct> current_struct;
    std::vector<Struct> all_structs;
    std::stringstream &ss;
    int lvl = 0;

  public:
    Parser(std::stringstream &ss) : ss(ss) {}

    int getLvl() {
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

    void skip_all_char(char ch) {
        std::stringstream::pos_type pos;
        do {
            pos = ss.tellg();
        } while (ss.get() == ch);
        ss.seekg(pos);
    }

    void skip_until(char ch) {
        char curr_ch;
        while (ss.peek() != ch && ss.peek() != -1) {
            curr_ch = ss.get();
        }
        ss.get();
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

    void parseLine(bool &is_struct_definition) {
        char ch;
        auto pos = ss.tellg();
        skip_all_char('-');
        if (checkEqual("CXXRecordDecl")) {
            if (!tryFind("implicit") && (tryFind("struct") || tryFind("class"))) {
                std::vector<std::string> args;
                std::string args_raw;
                while (ss.peek() != '\n') {
                    args_raw += ss.get();
                }
                args = split(args_raw, ' ');
                if (args.back() == "definition") {
                    args.pop_back();
                    if (args.back() != "struct" && args.back() != "class") {
                        current_struct.push_back({current_location, args.back()});
                        is_struct_definition = true;
                    }
                }
            }
        } else if (checkEqual("FieldDecl")) {
            std::vector<std::string> args;
            std::string args_raw;
            while (ss.peek() != '\n') {
                args_raw += ss.get();
            }
            args = split(args_raw, ' ');
            if (!current_struct.empty()) {
                current_struct.back().fields.push_back({args.at(args.size() - 2), args.back()});
            }
        } else if (checkEqual("AnnotateAttr")) {
            if (tryFind("\"reflectable\"")) {
                current_struct.back().is_reflectable = true;
            }
        }
        skip_until('\n');
        if (current_struct.empty() && is_struct_definition) {
            std::cout << "fuck\n";
        }
    }

    void parse(int level = 1) {
        do {
            // parse each line
            auto line_begin = ss.tellg();
            lvl = getLvl();

            if (lvl == level) {
                bool is_struct_definition = false;
                parseLine(is_struct_definition);
                line_begin = ss.tellg();
                if (is_struct_definition) {
                    auto &last_struct = current_struct.back();
                    std::cout << last_struct << "\n";
                    current_location.vec.push_back(last_struct.name);
                    ss.seekg(line_begin);
                    parse(level + 1);
                    current_location.vec.pop_back();
                    if (last_struct.is_reflectable)
                        all_structs.push_back(last_struct);
                    current_struct.pop_back();
                }
            } else {
                ss.seekg(line_begin);
                if (lvl > level) {
                    parse(level + 1);
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

    ofstream ast_file(headerFileName + "_ast");
    stringstream ss(exec("clang++" + additional_params + " -Xclang -ast-dump -fsyntax-only -fno-color-diagnostics -Wno-visibility '" + headerFile + "'"));
    ast_file << ss.str();
    ast_file.close();

    auto start = chrono::steady_clock::now();

    string built_component = "inline void BuildTypes(Meta::TypeMap &map) {";
    while (ss.get() != '\n') {
    };
    Parser parser(ss);
    parser.parse();
    parser.dump();
    built_component += "}\n";

    ofstream meta(headerFileName + "_meta.hpp");
    meta << built_component;
    meta.close();

    if (print_to_console) {
        cout << built_component;
    }

    cout << "\nBuilt in: " << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() << " ms\n";

    return 0;
}