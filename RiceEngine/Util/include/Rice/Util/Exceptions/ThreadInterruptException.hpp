#pragma once

#include "Exception.hpp"
#include "Rice/Util/namespaces.h"
#include <fmt/format.h>
#include <string>
#include <thread>

#define THROW_THREAD_ITERRUPT_EXCEPTION                                      \
    throw NSP_UTIL::ThreadInterruptException(__LINE__, __FILE__)

NSP_UTIL_BEGIN

class ThreadInterruptException : public Exception {
  public:
    ThreadInterruptException(int line_, const char *file_)
        : Exception(fmt::format("Thread [{}] was interrupted",
                                std::hash<std::thread::id>()(
                                    std::this_thread::get_id()))
                        .c_str(),
                    line_, file_) {}
};

NSP_UTIL_END
