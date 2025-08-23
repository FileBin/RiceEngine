#pragma once
#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

static String logLevel_str[] = {"", "INFO", "DEBUG", "WARNING", "ERROR", "CRITICAL_ERROR"};

class Log {
    struct Localization {
        String log_begin = "Log Created";
        String log_end = "Log End";
        String log_is_already_created = "Log is already created!";
        String log_creation_error = "Log creation-error!";
    } static llocale;

  public:
    static void init();

    enum LogLevel { None = 0, Info = 1, Debug = 2, Warning = 3, Error = 4, Critical = 5 };

    template <typename... Args>
    static void log(LogLevel logLevel, fmt::format_string<Args...> message, Args &&...args) {
        if (instance == nullptr)
            return;
        String str = fmt::format(message, std::forward<Args>(args)...);
        log(logLevel, str);
    }

    static void log(String message) { log(LogLevel::Info, message); }

    static void log(LogLevel logLevel, String message) {
        if (instance == nullptr)
            return;
        if (logLevel == LogLevel::Debug) {
#ifdef _DEBUG
            instance->print(logLevel_str[(int)logLevel], message);
#endif
        } else {
            instance->print(logLevel_str[(int)logLevel], message);
        }
    }

    template <class... Args>
    static void debug(fmt::format_string<Args...> message, Args &&...args) {
#ifdef _DEBUG
        if (instance == nullptr)
            return;
        log(Debug, message, std::forward<Args>(args)...);
#endif
    }

    static void close();
    ~Log();

  private:
    static uptr<Log> instance;
    Log();
    void _init();
    void _close();
    void print(String levtext, String text);

    std::fstream file;
};

NSP_ENGINE_END
