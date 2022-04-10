/*
 * Event.hpp
 *
 *  Created on: Apr 10, 2022
 *      Author: root
 */

#include "../stdafx.hpp"

#include "RegisterCollection.hpp"

#pragma once

template<typename ret_t = void, typename ...args_t>
class Event {
public:
	typedef ret_t func_t(args_t...);
	typedef _STD function<func_t> std_func_t;
	struct UUID {
	friend class Event;
	UUID() : coll_id(SIZE_MAX) {};
	private:
		size_t coll_id;
	};

private:
	RegisterCollection<std_func_t> handlers;

public:
	UUID subscribe(std_func_t function) {
		UUID uuid;
		uuid.coll_id = handlers.registerObj(function);
		return uuid;
	}

	UUID subscribe(func_t* function) {
		return subscribe([function]() { function(); });
	}

	void unsubscribe(UUID uuid) {
		handlers.unregister(uuid.coll_id);
	}

	void invoke(args_t... args) {
		auto& coll = handlers.getCollectionWithGaps();
		for (SmartPtr<std_func_t> func : coll) {
			if(func.isNull()) continue;
			(*func)(args...);
		}
	}
};
