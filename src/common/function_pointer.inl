#pragma once
#include "function_pointer.h"

namespace angler
{
	template<typename R, typename ...Args>
	inline Function<R, Args...>::Function(function _func)
	{
		m_pointer = new Function<R, Args...>::FunctionPointer(_func);
	}

	template<typename R, typename ...Args>
	template<class C>
	inline Function<R, Args...>::Function(C* _object, method<C> _func)
	{
		m_pointer = new Function<R, Args...>::MethodPointer<C>(_object, _func);
	}

	template<typename R, typename ...Args>
	template<class C>
	inline Function<R, Args...>::Function(C const* _object, methodConst<C> _func)
	{
		m_pointer = new Function<R, Args...>::MethodConstPointer<C>(_object, _func);
	}

	template<typename R, typename ...Args>
	inline Function<R, Args...>::Function(const Function<R, Args...>& _func)
	{
		delete m_pointer;
		m_pointer = _func.m_pointer->clone();
	}

	template<typename R, typename ...Args>
	inline Function<R, Args...>::Function(Function<R, Args...>&& _func) noexcept
	{
		delete m_pointer;
		m_pointer = _func.m_pointer->clone();
	}

	template<typename R, typename ...Args>
	inline Function<R, Args...>& Function<R, Args...>::operator=(const Function<R, Args...>& _func)
	{
		delete m_pointer;
		m_pointer = _func.m_pointer->clone();
		return *this;
	}

	template<typename R, typename ...Args>
	inline Function<R, Args...>& Function<R, Args...>::operator=(Function<R, Args...>&& _func) noexcept
	{
		delete m_pointer;
		m_pointer = _func.m_pointer->clone();
		return *this;
	}

	template<typename R, typename ...Args>
	inline R Function<R, Args...>::operator()(Args... _args)
	{
		return m_pointer->invoke(_args...);
	}

	template<typename R, typename ...Args>
	inline Function<R, Args...>::~Function()
	{
		delete m_pointer;
	}

	/////////////////////////////////////////////////////////

	template<typename R, typename ...Args>
	inline R Function<R, Args...>::FunctionPointer::invoke(Args ..._args)
	{
		return m_function(_args...);
	}

	template<typename R, typename ...Args>
	inline Function<R, Args...>::FunctionBase* Function<R, Args...>::FunctionPointer::clone()
	{
		return new FunctionPointer(*this);
	}

	/////////////////////////////////////////////////////////

	template<typename R, typename ...Args>
	template<class C>
	inline R Function<R, Args...>::MethodPointer<C>::invoke(Args ..._args)
	{
		return (m_object->*m_function)(_args...);
	}

	template<typename R, typename ...Args>
	template<class C>
	inline Function<R, Args...>::FunctionBase* Function<R, Args...>::MethodPointer<C>::clone()
	{
		return new MethodPointer<C>(*this);
	}

	/////////////////////////////////////////////////////////

	template<typename R, typename ...Args>
	template<class C>
	inline R Function<R, Args...>::MethodConstPointer<C>::invoke(Args ..._args)
	{
		return (m_object->*m_function)(_args...);
	}

	template<typename R, typename ...Args>
	template<class C>
	inline Function<R, Args...>::FunctionBase* Function<R, Args...>::MethodConstPointer<C>::clone()
	{
		return new MethodConstPointer<C>(*this);
	}
} 

