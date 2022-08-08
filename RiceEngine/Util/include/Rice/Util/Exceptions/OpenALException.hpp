#pragma once

#include "Exception.hpp"

#define THROW_OPENAL_EXCEPTION(wtf)                                            \
    throw : NSP_UTIL::OpenALException(wtf, __LINE__, __FILE__)

NSP_UTIL_BEGIN

class OpenALException : public Exception {
  public:
    OpenALException(const char *what, int line_, const char *file_)
        : Exception(what, line_, file_) {}
};

NSP_UTIL_END
