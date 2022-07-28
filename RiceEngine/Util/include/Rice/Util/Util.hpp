#include "stdafx.hpp"
#include "Interfaces.hpp"
#include "stdafx.hpp"
#include <cstdint>
#include <filesystem>
#include <limits>
#include <string>

#pragma once

struct UUID {
  private:
    uint64_t val;

    friend bool operator==(UUID one, UUID two);

  public:
    UUID() : val(std::numeric_limits<uint64_t>::max()) {}
    UUID(uint64_t val) : val(val) {}

    UUID &operator=(uint64_t v) {
        val = v;
        return *this;
    }

    UUID &operator=(const UUID &other) {
        val = other.val;
        return *this;
    }

    uint64_t getVal() { return val; }
};

inline bool operator==(UUID one, UUID two) { return one.val == two.val; }

NSP_UTIL_BEGIN

void checkPath(std::filesystem::path path);
data_t readFile(std::filesystem::path filename);
void writeFile(std::filesystem::path filename, data_t data);

bool getFromJson(std::filesystem::path filename, std::string key,
                 std::string &out_value);
bool getFromJson(std::filesystem::path filename, std::string key,
                 num &out_value);
bool getFromJson(std::filesystem::path filename, std::string key,
                 UUID &out_value);

void writeToJSON(std::filesystem::path filename, std::string key,
                 std::string value);
void writeToJSON(std::filesystem::path filename, std::string key, num value);
void writeToJSON(std::filesystem::path filename, std::string key, UUID value);
std::filesystem::path getHomeDirectory();
std::filesystem::path getAppDirectory();
NSP_UTIL_END
