#include "pch.h"
#include "clock.h"

namespace angler
{
	std::chrono::time_point<std::chrono::high_resolution_clock> Clock::getTime()
	{
		return std::chrono::high_resolution_clock::now();
	}

	void Clock::start()
	{
		lastPoint = std::chrono::high_resolution_clock::now();
	}
}