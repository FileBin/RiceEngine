#include "../stdafx.hpp"

#pragma once

NSP_ENGINE_BEGIN

template<typename T>
struct IPackable {
    virtual T pack() = 0;
};

NSP_ENGINE_END