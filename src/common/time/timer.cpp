#include "pch.h"
#include "timer.h"

namespace angler
{ 
	Timer::Timer(float _duration, bool _start, TimerBehavior _behavior) {
		m_running = _start;

		m_duration = _duration;
		m_time = _start ? m_duration : 0.0f;
		m_behavior = _behavior;
	}

	Timer::~Timer()
	{
		onCompleted.clearListeners();
	}

	void Timer::setDuration(float _duration) {
		m_duration = _duration;
	}

	void Timer::resume() {
		m_running = true;
	}

	void Timer::start() {
		resume();
	}

	void Timer::pause() {
		m_running = false;
	}

	void Timer::stop() {
		pause();
	}

	void Timer::restart() {
		m_time = 0.0f;
	}

	bool Timer::isCompleted() const {
		return m_time >= m_duration;
	}

	bool Timer::isRunning() const {
		return m_running;
	}

	void Timer::update(float _dt) {
		if (!m_running)
			return;

		if (m_time >= m_duration) {
			switch (m_behavior) {
			case TimerBehavior::AutoRestart:
				restart(); break;
			case TimerBehavior::Wait:
				pause(); break;
			default:
				break;
			}

			onCompleted.invoke();
			return;
		}

		m_time += _dt;
	}
}
