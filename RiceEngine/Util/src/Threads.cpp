#include "Rice/defines.h"

#include "Rice/Util/Threads.hpp"

#include "Rice/Util/namespaces.h"
#include "SDL2/SDL_thread.h"

#include <pthread.h>

NSP_UTIL_BEGIN

void setCurrentThreadPriority(ThreadPriority priority) {
    switch (priority) {
    case ThreadPriority::Low:
        SDL_SetThreadPriority(SDL_THREAD_PRIORITY_LOW);
        break;
    case ThreadPriority::Normal:
        SDL_SetThreadPriority(SDL_THREAD_PRIORITY_NORMAL);
        break;
    case ThreadPriority::High:
        SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);
        break;
    case ThreadPriority::TimeCritical:
        SDL_SetThreadPriority(SDL_THREAD_PRIORITY_TIME_CRITICAL);
        break;
    }
}

NSP_UTIL_END
