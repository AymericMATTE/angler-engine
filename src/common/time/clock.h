#pragma once
#include <chrono>

namespace angler
{
	class Clock
	{
	public :
		using seconds = std::chrono::seconds::period;
		using milliseconds = std::chrono::milliseconds::period;
		using microseconds = std::chrono::microseconds::period;
		using nanoseconds = std::chrono::nanoseconds::period;

		[[nodiscard]] static std::chrono::time_point<std::chrono::high_resolution_clock> getTime();

		void start();

		template<class T>
		double reset();

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> lastPoint;
	};
}

#include "clock.inl"