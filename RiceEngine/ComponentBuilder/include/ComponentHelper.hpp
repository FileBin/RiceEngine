#include "SerializableComponent.hpp"
#include "serialization_helper.hpp"
#include <MetaCompiler/ReflectionHelper.hpp>

#define PPCAT_NX(A, B) A##B
#define PPCAT(A, B) PPCAT_NX(A, B)

#define REFLECTABLE __attribute__((annotate("reflectable")))
#define RANGE(_min, _max) __attribute__((annotate("range:" #_min "," #_max)))

#define PPCAT_NX(A, B) A##B
#define PPCAT(A, B) PPCAT_NX(A, B)

#define RICE_COMPONENT                                                                                                 \
    template <typename T> friend struct ::Meta::TypeOf;                                                                \
    template <typename T> friend struct ::serializer;