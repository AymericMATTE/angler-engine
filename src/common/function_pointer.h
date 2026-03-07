#pragma once

namespace angler
{
    template<typename R, typename ...Args>
    class Function
    {
    public:
        using function = R(*)(Args...);
        template <class C>
        using method = R(C::*)(Args...);
        template <class C>
        using methodConst = R(C::*)(Args...) const;

		Function() = default;

		Function(function _func);

		template <class C>
		Function(C* _object, method<C> _func);

		template <class C>
		Function(C const* _object, methodConst<C> _func);

		Function(const Function<R, Args...>& _func);
		Function(Function<R, Args...>&& _func) noexcept;
		Function<R, Args...>& operator=(const Function<R, Args...>& _func);
		Function<R, Args...>& operator=(Function<R, Args...>&& _func) noexcept;

		R operator()(Args... _args);

		virtual ~Function();

    private:
		struct FunctionBase;
		struct FunctionPointer;
		template<class C>
		struct MethodPointer;
		template<class C>
		struct MethodConstPointer;

		FunctionBase* m_pointer = nullptr;
    };

	template<typename R, typename ...Args>
	struct Function<R, Args...>::FunctionBase
	{
		FunctionBase() = default;
		virtual ~FunctionBase() = default;

		virtual R invoke(Args... _args) = 0;
		virtual FunctionBase* clone() = 0;
	};

	template<typename R, typename ...Args>
	struct Function<R, Args...>::FunctionPointer : public FunctionBase
	{
		FunctionPointer(Function::function _func) : m_function(_func) {}
		virtual ~FunctionPointer() = default;

		R invoke(Args... _args) override;
		FunctionBase* clone() override;

	private:
		Function::function m_function = nullptr;
	};

	template<typename R, typename ...Args>
	template <class C>
	struct Function<R, Args...>::MethodPointer : public FunctionBase
	{
		MethodPointer(C* _object, Function::method<C> _func) : m_object(_object), m_function(_func) {}
		virtual ~MethodPointer() = default;

		R invoke(Args... _args) override;
		FunctionBase* clone() override;

	private:
		Function::method<C> m_function = nullptr;
		C* m_object = nullptr;
	};

	template<typename R, typename ...Args>
	template <class C>
	struct Function<R, Args...>::MethodConstPointer : public FunctionBase
	{
		MethodConstPointer(C const* _object, Function::methodConst<C> _func) : m_object(_object), m_function(_func) {}
		virtual ~MethodConstPointer() = default;

		R invoke(Args... _args) override;
		FunctionBase* clone() override;

	private:
		Function::methodConst<C> m_function = nullptr;
		C const* m_object = nullptr;
	};
}
#include "function_pointer.inl"