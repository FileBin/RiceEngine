#include "Rice/Util/namespaces.h"
#include "stdafx.hpp"

NSP_UTIL_BEGIN

enum class ThreadPriority {
    Low,
    Normal,
    High,
    TimeCritical
};

void setCurrentThreadPriority(ThreadPriority priority);

NSP_UTIL_END