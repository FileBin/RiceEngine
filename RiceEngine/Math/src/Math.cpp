#include "pch.h"
#include "Rice/Math/Math.hpp"

NSP_ENGINE_BEGIN

/*Vector3 Math::GetNorm(_STD function<double(Vector3)> func, Vector3 pos, dbl e) {
    //dbl d = func(pos);
    //return Vector3(func(pos + Vector3::right * e) - d, func(pos + Vector3::up * e) - d, func(pos + Vector3::forward * e) - d).Normalized();

    return Vector3(func(pos + Vector3::right * e) - func(pos - Vector3::right * e), func(pos + Vector3::up * e) - func(pos - Vector3::up * e), func(pos + Vector3::forward * e) - func(pos - Vector3::forward * e)).Normalized();
}*/

NSP_ENGINE_END
