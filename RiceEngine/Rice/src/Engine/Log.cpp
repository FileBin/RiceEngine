#include <Rice/Engine/Log.hpp>
#include <ios>
#include <iostream>
#include <locale.h>

#include <codecvt>

#define LOGNAME "latest.log"

using namespace std::chrono;

steady_clock::time_point start;

String get_date() {
    return "";
    // using namespace std::chrono;
    // auto t = std::time(0);
    // std::tm time = *std::localtime(&t);
    // wchar_t buf[0x100];
    // wcsftime(buf, 0x100, L"%X %x %Z", &time);
    // return buf;
    // return std::to_string(std::put_time(&time, "%c %Z"));
    /*return fmt::format(L"{}:{}:{}, {:%A}, {} {}, {}", // @suppress("Invalid arguments")
                    time->tm_hour, time->tm_min, time->tm_sec,
                    time->tm_wday,
                    time->tm_mday, time->tm_mon,
                    time->tm_year); */
}

String get_timer() {
    double time =
        duration_cast<microseconds>(steady_clock::now() - start).count() / 1000. / 1000.;
    return fmt::format("{:.4f}s", time);
}

NSP_ENGINE_BEGIN

uptr<Log> Log::instance = nullptr;
Log::Localization Log::llocale = {};

void Log::init() {
    if (!instance) {
        instance.reset(new Log());
    } else
        log(LogLevel::Error, llocale.log_is_already_created);
}

Log::Log() {
    start = std::chrono::steady_clock::now();
    _init();
}

Log::~Log() { _close(); }

void Log::close() { delete instance.release(); }

void Log::_init() {
    using namespace std;
    file.open(LOGNAME, fstream::out | fstream::trunc);
#ifdef _MSC_VER
    auto utf8 = std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>);
#else
    auto utf8 = locale(file.getloc(), new std::codecvt_utf8<wchar_t>());
#endif // _MSC_VER

    file.imbue(utf8);
    if (file.is_open()) {
        file << fmt::format("{}: {}.\n---------------------------------------\n\n",
                            llocale.log_begin, get_date());
    } else {
        printf(llocale.log_creation_error.c_str());
    }
}

void Log::_close() {
    if (!file || !file.is_open())
        return;

    file << fmt::format(
        "\n---------------------------------------\n{}: {}\nTotal execution time: {}",
        llocale.log_end, get_date(), get_timer());
    file.flush();
    file.close();
}

void Log::print(String levtext, String text) {

    String str = fmt::format("{}: [{}] {}\n", get_timer(), levtext, text);

    std::cout << str.c_str();
    fflush(stdout);
    file << str.c_str();
#ifdef _DEBUG
    file.flush();
#endif
}

NSP_ENGINE_END
