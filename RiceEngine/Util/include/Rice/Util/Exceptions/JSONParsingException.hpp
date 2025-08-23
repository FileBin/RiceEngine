#pragma once

#include "Exception.hpp"
#include "Rice/Util/namespaces.h"

#define THROW_JSON_PARSING_EXCEPTION(JsonFile, offset)                                        \
    throw NSP_UTIL::JSONParsingException(JsonFile, offset, __LINE__, __FILE__)

NSP_UTIL_BEGIN

class JSONParsingException : public Exception {
  private:
    size_t offset = 0;
    const char *file;

  public:
    JSONParsingException(const char *jsonFile, size_t offset_, int line_, const char *file_);

    size_t getOffset();
    String getFile();
};

NSP_UTIL_END
