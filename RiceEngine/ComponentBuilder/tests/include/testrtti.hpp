#include <memory>
#include <string>

struct A {
    std::string str;
};

class Component {
  public:
    int x, y, z;
    A a;

  private:
    bool u;
    std::unique_ptr<std::string> b;
};