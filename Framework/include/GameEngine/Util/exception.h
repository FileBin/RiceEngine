#include <exception>
#include <string>

namespace Game {
    class exception : public std::exception {
        int line;
        const char* func;
        const char* info;

    public:
        exception(const char* msg, int line_, const char* func_, const char* info_ = "no additional info available") : std::exception(msg),
            line(line_),
            func(func_),
            info(info_)
        {
        }

        int get_line() const { return line; }
        const char* get_func() const { return func; }
        const char* get_info() const { return info; }

    };
}