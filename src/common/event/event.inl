#pragma once
#include "event.h"
#include <cassert>

namespace angler
{
	template<typename ...Args>
	inline void Event<Args...>::invoke(Args... _args) {
		for (Listener& listener : m_listeners) {
			if (listener.m_isConnected == false)
				continue;

			listener.m_listener(_args...);
		}
	}

	template<typename ...Args>
	inline Event<Args...>::Connection Event<Args...>::connect(function _func) {
		Function<void, Args...> fctListener(_func);
		uint id = addListenerBase(fctListener);
		return Connection(this, id);
	}

	template<typename ...Args>
	template<class C>
	inline Event<Args...>::Connection Event<Args...>::connect(C* _object, method<C> _func) {
		Function<void, Args...> methodListener(_object, _func);
		uint id = addListenerBase(methodListener);
		return Connection(this, id);
	}

	template<typename ...Args>
	template<class C>
	inline Event<Args...>::Connection Event<Args...>::connect(C const* _object, methodConst<C> _func) {
		Function<void, Args...> methodConstListener(_object, _func);
		uint id = addListenerBase(methodConstListener);
		return Connection(this, id);
	}

	template<typename ...Args>
	inline void Event<Args...>::disconnect(uint _id) {
		assert(_id < m_listeners.size());

		if (m_listeners[_id].m_isConnected == false)
			return;

		m_listeners[_id].m_isConnected = false;
		m_idQueue.push(_id);
	}

	template<typename ...Args>
	inline void Event<Args...>::clearListeners() {
		for (uint i = 0; i < m_listeners.size(); i++) {
			if (m_listeners[i].m_isConnected == false)
				continue;

			m_listeners[i].m_isConnected = false;
			m_idQueue.push(i);
		}
	}

	template<typename ...Args>
	inline uint Event<Args...>::addListenerBase(Function<void, Args...>& _func) {
		if (m_idQueue.empty()) {
			m_listeners.push_back({ _func, true });
			return static_cast<uint>(m_listeners.size() - 1);
		}

		uint result = m_idQueue.front();
		m_idQueue.pop();
		m_listeners[result] = { _func, true };

		return result;
	}

	template<typename ...Args>
	inline void Event<Args...>::Connection::disconnect() {
		m_event->disconnect(m_id);
	}

	template<typename ...Args>
	Event<Args...>::Connection::Connection(Event* _event, uint _id) {
		m_event = _event;
		m_id = _id;
	}
}