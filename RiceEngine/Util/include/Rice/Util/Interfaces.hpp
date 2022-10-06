#include "stdafx.hpp"

#pragma once

NSP_ENGINE_BEGIN

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