#include "stdafx.hpp"

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#pragma once

NSP_ENGINE_BEGIN

struct ISerializable {
    virtual data_t toBytes() = 0;
    virtual ~ISerializable() {}
};

struct IJsonSerializable {
    virtual nlohmann::json toJson() = 0;
    virtual ~IJsonSerializable() {}
};

struct IRegistrable {
    virtual void Register(size_t idx) = 0;
    virtual size_t Unregister() = 0;
    virtual ~IRegistrable() {}
};

struct ICleanable {
    virtual void cleanup() = 0;
    virtual ~ICleanable() = 0;
};

inline ICleanable::~ICleanable() {}

NSP_ENGINE_END