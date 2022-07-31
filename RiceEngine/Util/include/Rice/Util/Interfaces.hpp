#include "stdafx.hpp"

#include "ByteStream.hpp"

#pragma once


NSP_ENGINE_BEGIN

template<typename T>
struct IPackable {
    virtual T pack() = 0;
};

struct IPacker {
    virtual void pack(Util::ByteStream& stream) = 0;
    virtual void unpack(Util::ByteStream& stream) = 0;
};

struct IBinaryPackable {
    virtual ptr<IPacker> getPacker() = 0;
    data_t pack();
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