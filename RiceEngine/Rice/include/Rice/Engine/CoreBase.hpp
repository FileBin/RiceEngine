#include "Rice/Util/Exceptions/ThreadInterruptException.hpp"
#include "Rice/stdafx.hpp"

NSP_ENGINE_BEGIN
class CoreBase;
class SceneBase;
NSP_ENGINE_END

#pragma once
#include "Log.hpp"

#include "EngineBase.hpp"

NSP_ENGINE_BEGIN

class CoreBase {
  private:
    template <typename RetT, typename Signature, typename... ArgsT>
    static RetT executeFuncAndHandleExceptions(std::function<Signature> &fn, ArgsT... args) {
#ifdef NDEBUG
        try {
            return fn(args...);
        } catch (Util::Exception &e) {
            Log::log(Log::Error, "Not handled exception occured: {}",
                     String(typeid(e).name()));
            Log::log(Log::Error, "Line {}", std::to_wstring(e.line()));
            Log::log(Log::Error, "File: {}", e.file());
            Log::log(Log::Error, "Message: {}", e.msg());
            Log::log(Log::Error, "AdditionalInfo: {}\n", e.info());
            Log::log(Log::Error, "Stack trace:\n{}", e.stack());
            Log::close();
            throw e;
        } catch (std::exception &e) {
            Log::log(Log::Error, "std::exception occured: {}", String(typeid(e).name()));
            Log::log(Log::Error, "What: {}", String(e.what()));
            Log::close();
            throw e;
        }
#else
        return fn(args...);
#endif
    }

  public:
    template <typename Signature, typename... ArgsT>
    static ptr<std::jthread> runThread(std::function<Signature> func, ArgsT... args) {
        return ptr<std::jthread>(new std::jthread(
            [](std::stop_token _St, std::function<Signature> _Fx, ArgsT... _Ax) {
                setCurrentThreadPriority(ThreadPriority::Normal);
                using cds::threading::Manager;

                if (!Manager::isThreadAttached())
                    Manager::attachThread();

                auto f_wrp = [&_Fx](ArgsT... args) {
                    try {
                        _Fx(args...);
                    } catch (Util::ThreadInterruptException e) {
                        Log::debug("Thread interrupted");
                    }
                };

                executeFuncAndHandleExceptions<void, Signature>(f_wrp, _Ax..., _St);
                Manager::detachThread();
            },
            func, args...));
    }

    static void runTask(std::function<void(void)> func) {
        // TODO make tasks
        func(); // temporary replacement
    }

    virtual void loadScene(ptr<SceneBase> scene) = 0;

    dbl getFixedDeltaTime() { return fixedDeltaTime; }
    dbl getDeltaTime() { return deltaTime; }
    dbl getTime() { return time; }

  protected:
    CoreBase();

  public:
    ~CoreBase();

  protected:
    friend class EngineBase;

    ptr<EngineBase> engine;
    dbl update_rate = 60.;
    dbl fixedDeltaTime = 0, deltaTime = 0, time = 0;
    bool is_init = false;
};

NSP_ENGINE_END
