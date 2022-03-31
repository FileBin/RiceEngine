#pragma once

__interface IRegistrable {
	void Register(size_t idx);
	size_t Unregister();
};