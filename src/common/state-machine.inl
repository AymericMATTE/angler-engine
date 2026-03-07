#pragma once
#include "state-machine.h"
#include <cassert>

namespace angler
{
	template<typename T>
	inline StateMachine<T>::StateMachine(T* _object) {
		m_object = _object;
	}

	template<typename T>
	inline void StateMachine<T>::setObject(T* _object) {
		m_object = _object;
	}

	template<typename T>
	inline std::string StateMachine<T>::getActualState() const {
		return m_actualState;
	}

	///////////////////////////////////////////////////////////////////////////////
	// @brief Adds a given action to the state machine. (Single Action version)
	// @param name The name of the action. 
	// @param action The action to be added.
	///////////////////////////////////////////////////////////////////////////////
	template<typename T>
	inline void StateMachine<T>::addState(const std::string& _name, State _state) {
		m_states[_name] = _state;

	}

	template<typename T>
	inline void StateMachine<T>::addState(std::string&& _name, State _state) {
		addState(_name, _state);
	}

	///////////////////////////////////////////////////////////////////////////////////
	// @brief Sets the OnBegin action for a given action in the state machine.
	// @param name The name of the action. 
	// @param pOnBegin Pointer to the function to be called when the action begins.
	///////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	inline void StateMachine<T>::setOnBeginState(const std::string& _name, OnBegin _onBegin) {
		assert(m_states.contains(_name) == true);
		m_states[_name].onBegin = _onBegin;
	}

	template<typename T>
	inline void StateMachine<T>::setOnBeginState(std::string&& _name, OnBegin _onBegin) {
		setOnBeginState(_name, _onBegin);
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// @brief Sets the OnUpdate action for a given action in the state machine.
	// @param name The name of the action.
	// @param pOnUpdate Pointer to the function to be called when the action is updated.
	////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	inline void StateMachine<T>::setOnUpdateState(const std::string& _name, OnUpdate _onUpdate) {
		assert(m_states.contains(_name) == true);
		m_states[_name].onUpdate = _onUpdate;
	}

	template<typename T>
	inline void StateMachine<T>::setOnUpdateState(std::string&& _name, OnUpdate _onUpdate) {
		setOnUpdateState(_name, _onUpdate);
	}

	///////////////////////////////////////////////////////////////////////////////
	// @brief Sets the OnEnd action for a given action in the state machine.
	// @param name The name of the action.
	// @param pOnEnd Pointer to the function to be called when the action ends.
	///////////////////////////////////////////////////////////////////////////////
	template<typename T>
	inline void StateMachine<T>::setOnEndState(const std::string& _name, OnEnd _onEnd) {
		assert(m_states.contains(_name) == true);
		m_states[_name].onEnd = _onEnd;
	}

	template<typename T>
	inline void StateMachine<T>::setOnEndState(std::string&& _name, OnEnd _onEnd) {
		setOnEndState(_name, _onEnd);
	}

	////////////////////////////////////////////////////
	// @brief Add a transition to the state machine.
	// @param transition The transition to be added.
	////////////////////////////////////////////////////
	template<typename T>
	inline void StateMachine<T>::addTransition(const Transition& _transition) {
		m_transitions.push_back(_transition);
	}

	template<typename T>
	inline void StateMachine<T>::addTransition(Transition&& _transition) {
		m_transitions.push_back(_transition);
	}

	///////////////////////////////////////
	// @brief Update the state machine.
	///////////////////////////////////////
	template<typename T>
	void StateMachine<T>::update() {
		if (m_actualState == "" || m_object == nullptr)
			return;

		for (Transition& transition : m_transitions) {
			bool shouldTransit = true;

			if (transition.target == m_actualState)
				continue;

			for (Condition& condition : transition.conditions) {
				shouldTransit = shouldTransit && condition(m_object);
			}

			if (shouldTransit)
				return transit(transition.target);
		}

		if (m_states[m_actualState].onUpdate != nullptr)
			m_states[m_actualState].onUpdate(m_object);
	}

	///////////////////////////////////////////////////////////////////////
	// @brief Transits the state machine to a target action.
	// @param target The name of the targeted action to transition to.
	//////////////////////////////////////////////////////////////////////
	template<typename T>
	void StateMachine<T>::transit(const std::string& _target) {
		assert(m_states.contains(_target) == true);
		assert(m_object != nullptr);

		if (m_states.contains(m_actualState) && m_states[m_actualState].onEnd != nullptr)
			m_states[m_actualState].onEnd(m_object);

		m_actualState = _target;

		if (m_states[m_actualState].onBegin != nullptr)
			m_states[m_actualState].onBegin(m_object);
	}

	template<typename T>
	void StateMachine<T>::transit(std::string&& _target) {
		transit(_target);
	}
}