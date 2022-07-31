#include "stdafx.hpp"

#include "String.hpp"
#include "Exceptions.hpp"
#include "Event.hpp"
#include "Interfaces.hpp"
#include "ByteStream.hpp"
#include "Packer.hpp"

#include <cstdint>
#include <filesystem>
#include <limits>
#include <string>
#include <type_traits>

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

template <typename T> data_t pack(T t) {
    if constexpr (std::is_base_of<IPackable<data_t>, T>::value) {
        return t.pack();
    }
    constexpr auto l = sizeof(T);
    data_t data(l);
    memcpy(data.data(), &t, l);
    return data;
}

template <typename T> T unpack(data_t data) {
    if constexpr (std::is_base_of<IPackable<data_t>, T>::value) {
        return T::unpack(data);
    }
    constexpr auto l = sizeof(T);
    T t;
    memcpy(&t, data.data(), l);
    return t;
}

NSP_UTIL_END
