#pragma once
#include "clock.h"

namespace angler
{
	template<class T>
	inline double Clock::reset()
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> newPoint = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, T> delta = newPoint - lastPoint;
		lastPoint = newPoint;

		return delta.count();
	}
}