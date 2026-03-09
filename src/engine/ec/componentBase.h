#pragma once
#include <string_view>

using uint64 = unsigned long long;
using uint = unsigned int;

template<typename T>
constexpr std::string_view type_name() {
#if defined(__clang__) || defined(__GNUC__)
	std::string_view p = __PRETTY_FUNCTION__;
	auto start = p.find("T = ") + 4;
	auto end = p.find(']', start);
	return p.substr(start, end - start);
#elif defined(_MSC_VER)
	std::string_view p = __FUNCSIG__;
	auto start = p.find("type_name<") + 10;
	auto end = p.find(">(void)", start);
	return p.substr(start, end - start);
#else
	static_assert(false, "Unsupported compiler");
#endif
}

constexpr uint64 hash(std::string_view s)
{
	uint64 h = 14695981039346656037ull;
	for (char c : s)
	{
		h ^= static_cast<uint64>(c);
		h *= 1099511628211ull;
	}
	return h;
}

namespace angler
{
	class GameObject;

	struct Component
	{
		virtual void OnAwake() {}
		virtual void OnStart() {}
		virtual void OnUpdate() {}
		virtual void OnPreRender() {}
		virtual void OnRender() {}
		virtual void OnDestroy() {}

		//virtual void OnCollisionEnter() {}
		virtual void OnCollisionStay() {}
		//virtual void OnCollisionExit() {}

		[[nodiscard]] virtual uint64 Id() const = 0;

		[[nodiscard]] GameObject* getOwner() const { return m_owner; }

	private :
		GameObject* m_owner = nullptr;
		bool m_hasStarted = false;

		friend class GameObject;
	};

	template<typename Derived>
	struct ComponentBase : public Component
	{
		[[nodiscard]] uint64 Id() const override { return StaticId(); }
		[[nodiscard]] static constexpr uint64 StaticId() { return hash(type_name<Derived>()); }
	};
}