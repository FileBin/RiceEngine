#include "../stdafx.hpp"
#pragma once

NSP_ENGINE_BEGIN

interface IRegistrable {
	virtual void Register(size_t idx) = 0;
	virtual size_t Unregister() = 0;
	virtual ~IRegistrable() {}
};

NSP_ENGINE_END
