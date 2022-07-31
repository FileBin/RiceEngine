/*
 * Event.hpp
 *
 *  Created on: Apr 10, 2022
 *      Author: root
 */

#include "Rice/Util/namespaces.h"
#include "stdafx.hpp"

#include "Interfaces.hpp"

#include "RegisterCollection.hpp"

#pragma once

NSP_UTIL_BEGIN
struct EventRegistration;

class EventBase : public enable_ptr<EventBase> {
  protected:
    friend struct EventRegistration;
    ptr<EventRegistration> createRegistration(uint id);
    uint getID(EventRegistration &reg);
    void check(EventRegistration &reg);
    virtual void unregister(uint id) = 0;

  public:
    virtual ~EventBase() {}
};

struct EventRegistration : public ICleanable {
    friend class EventBase;
    constexpr static uint unregistered_id = 0xffffffff;
    EventRegistration(ptr<EventBase> event_ref)
        : event_ref(event_ref), coll_id(unregistered_id){};

  private:
    uint coll_id;
    ptr<EventBase> event_ref;

  public:
    EventRegistration() : event_ref(nullptr), coll_id(unregistered_id) {}
    ~EventRegistration() { cleanup(); }
    EventRegistration(const EventRegistration &other) = delete;

    void move(EventRegistration &other);
    bool isRegistered() { return coll_id != unregistered_id; }

    const EventRegistration &operator=(const EventRegistration &other) {
        cleanup();
        event_ref = other.event_ref;
        coll_id = other.coll_id;
        return *this;
    }

    void cleanup() override;
};

template <typename... args_t> class Event : virtual public EventBase {
  public:
    typedef void func_t(args_t...);
    typedef std::function<func_t> std_func_t;

  private:
    Event() = default;
    RegisterCollection<std_func_t> handlers;

  protected:
    void unregister(uint id) override { handlers.unregister(id); }

  public:
    static ptr<Event<args_t...>> create() {
        return ptr<Event<args_t...>>{ new Event<args_t...>() };
    }

    void subscribe(EventRegistration &reg, std_func_t function) {
        createRegistration(handlers.registerObj(function))->move(reg);
    }

    void subscribe(EventRegistration &reg, func_t *function) {
        subscribe(reg, [function]() { function(); });
    }

    void unsubscribe(EventRegistration &reg) { reg.cleanup(); }

    void invoke(args_t... args) {
        const auto &coll = handlers.getCollectionWithGaps();

        // auto en = coll.getEnumeratorT();
        // while(en->next()) {
        for (auto &func : coll) {
            if (!func)
                return;
            (*func)(args...);
        }
        //}
    }
};

NSP_UTIL_END
