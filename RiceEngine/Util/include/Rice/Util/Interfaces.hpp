#include "stdafx.hpp"

#pragma once

NSP_ENGINE_BEGIN

template<typename T>
struct IPackable {
    virtual T pack() = 0;
};

struct IRegistrable {
	virtual void Register(size_t idx) = 0;
	virtual size_t Unregister() = 0;
	virtual ~IRegistrable() {}
};


struct ICleanable {
    virtual void cleanup() = 0;
    virtual ~ICleanable() {};
};

NSP_ENGINE_END