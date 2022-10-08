
int main() {
    using json = nlohmann::json;

    std::ifstream f("./res/ast.json");
    json data = json::parse(f);

    std::cout << data;

    return 0;
}