/*
 * List.cpp
 *
 *  Created on: May 1, 2022
 *      Author: filebin
 */

#include "Rice/Util/Util.hpp"

NSP_UTIL_BEGIN

void EventRegistration::cleanup() {
	if(event_ref) {
		event_ref->unregister(coll_id);
		coll_id = unregistered_id;
		event_ref = nullptr;
	}
}

void EventRegistration::move(EventRegistration& other) {
	other.coll_id = coll_id;
	other.event_ref = event_ref;
	coll_id = unregistered_id;
	event_ref = nullptr;
}

ptr<EventRegistration> EventBase::createRegistration(uint id) {
	ptr<EventRegistration> reg{ new EventRegistration(shared_from_this()) };
	reg->coll_id = id;
	return reg;
}

uint EventBase::getID(EventRegistration& reg) { return reg.coll_id; }

void EventBase::check(EventRegistration& reg) {
	if(reg.event_ref.get() != this)
		THROW_EXCEPTION("Event id doesn't match this event! (maybe you choose wrong event?)");
}

NSP_UTIL_END


