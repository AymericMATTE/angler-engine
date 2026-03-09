#pragma once
#include <vector>
#include <queue>
#include "../function_pointer.h"
#include "common/types.h"

namespace angler
{
	template<typename ...Args>
	class Event
	{
	public :
		using function = void(*)(Args...);
		template <class C> 
		using method = void(C::*)(Args...);
		template <class C> 
		using methodConst = void(C::*)(Args...) const;

		Event() = default;

		~Event() { clearListeners(); m_idQueue = {}; }

		struct Connection
		{
			void disconnect();
			~Connection() = default;
		private:
			Connection(Event* _event, uint _id);
			Event* m_event = nullptr;
			uint m_id = 0;

			friend class Event<Args...>;
		};

		void invoke(Args...);

		Connection connect(function _func);

		template<class C>
		Connection connect(C* _object, method<C> _func);

		template<class C>
		Connection connect(C const* _object, methodConst<C> _func);

		void disconnect(uint id);
		
		void clearListeners();

	private :
		struct Listener
		{
			Function<void, Args...> m_listener;
			bool m_isConnected = true;
		};

		uint addListenerBase(Function<void, Args...>& _func);

		std::vector<Listener> m_listeners;
		std::queue<uint> m_idQueue;
	};
}

#include "event.inl"