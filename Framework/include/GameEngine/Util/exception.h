
#include <exception>
#include <string>

namespace Game {
    class exception : public std::exception {
        int line;
        const wchar_t* func;
        const wchar_t* info;

    public:
        exception(const char* msg, int line_, const wchar_t* func_, const wchar_t* info_ = L"no additional info available") : std::exception(msg),
            line(line_),
            func(func_),
            info(info_)
        {
        }

        int get_line() const { return line; }
        const wchar_t* get_func() const { return func; }
        const wchar_t* get_info() const { return info; }

    };
}