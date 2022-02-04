﻿#include "pch.h"
#include <GameEngine\Math.h>

const double Game::Math::PI = 3.14159265359;

Vector3 Game::Math::GetNorm(std::function<double(Vector3)> func, Vector3 pos, dbl e) {
    //dbl d = func(pos);
    //return Vector3(func(pos + Vector3::right * e) - d, func(pos + Vector3::up * e) - d, func(pos + Vector3::forward * e) - d).Normalized();

    return Vector3(func(pos + Vector3::right * e) - func(pos - Vector3::right * e), func(pos + Vector3::up * e) - func(pos - Vector3::up * e), func(pos + Vector3::forward * e) - func(pos - Vector3::forward * e)).Normalized();
}
