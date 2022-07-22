#include "Rice/defines.h"
#include "pch.h"

#include "BetterCpp/Objects/String.hpp"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/writer.h"

//#include <codecvt>

#include <Rice/Util.hpp>

#include <filesystem>
#include <fstream>
#include <ios>
#include <string>

namespace fs = std::filesystem;

NSP_UTIL_BEGIN

void checkPath(fs::path path) {
    if (!std::filesystem::exists(
            path)) { // @suppress("Function cannot be resolved")
        THROW_FILE_NOT_FOUND_EXCEPTION(String(path));
    }
}

data_t readFile(fs::path filename) {
    checkPath(filename);
    std::ifstream infile(filename, std::ios_base::binary);
    auto data = data_t(std::istreambuf_iterator<char>(infile),
                       std::istreambuf_iterator<char>());
    return data;
}

template <typename T>
void writeToJSONT(fs::path filename, std::string key, T value) {
    using namespace rapidjson;
    using namespace std;
    fstream file;
    file.open(filename, fstream::in); // open file

    Document d;
    IStreamWrapper isw(file);
    d.ParseStream(isw); // parse file contents

    auto &alloc = d.GetAllocator();

    rapidjson::Value keyPart;
    keyPart.SetString(key.c_str(), alloc);

    rapidjson::Value valuePart;
    keyPart.Set(value, alloc);

    d.AddMember(keyPart, valuePart, alloc); // modify JSON data

    file.close(); // close file
    file.open(filename,
              fstream::out | fstream::trunc); // reopen it in truncate mode

    OStreamWrapper osw(file);
    d.Accept(osw); // rewrite file contents
    file.flush();
    file.close();
}

template <typename T>
bool getFromJsonT(std::filesystem::path filename, std::string key, T &out_val) {
    using namespace rapidjson;
    using namespace std;
    fstream file(filename, fstream::in);

    if (!file.is_open())
        return false;

    Document d;
    IStreamWrapper isw(file);
    d.ParseStream(isw); // parse file contents
    auto &v = d[key.c_str()];
    if (v.IsNull())
        return false;
    out_val = v.Get<T>();
    return true;
}

bool getFromJson(std::filesystem::path filename, std::string key,
                 std::string &out_value) {
    return getFromJsonT(filename, key, out_value);
}

bool getFromJson(std::filesystem::path filename, std::string key,
                 num &out_value) {
    return getFromJsonT(filename, key, out_value);
}

bool getFromJson(std::filesystem::path filename, std::string key,
                 UUID &out_value) {
    uint64_t val;
    auto ret = getFromJsonT(filename, key, val);
    out_value = UUID(val);
    return ret;
}

void writeToJSON(std::filesystem::path filename, std::string key,
                 std::string value) {
    writeToJSONT(filename, key, value);
}

void writeToJSON(std::filesystem::path filename, std::string key, num value) {
    writeToJSONT(filename, key, value);
}

void writeToJSON(std::filesystem::path filename, std::string key, UUID value) {
    writeToJSONT(filename, key, value.getVal());
}

fs::path getHomeDirectory() {
#ifdef _WIN32
    return fs::path(getenv("HOMEDRIVE")) / fs::path(getenv("HOMEPATH"));
#else
    return getenv("HOME");
#endif
}

fs::path getAppDirectory() { return getHomeDirectory() / APPNAME; }

NSP_UTIL_END
