#pragma once
#include "common/event/event.h"

namespace angler
{
	class Timer
	{
	public :
		enum class TimerBehavior {
			AutoRestart, // Automatically restart the timer once completed
			Wait // Wait until manually restarted
		};

		Timer() = default;
		Timer(float _duration, bool _start = false, TimerBehavior _behavior = TimerBehavior::AutoRestart);

		virtual ~Timer();

		void setDuration(float _duration);

		void resume();
		void start();

		void pause();
		void stop();

		void reset();
		void restart();

		bool isCompleted() const;

		bool isRunning() const;

		void update(float _dt);

		Event<> onCompleted;
	private :
		TimerBehavior m_behavior;

		float m_duration;
		float m_time;

		bool m_running;

	};
}