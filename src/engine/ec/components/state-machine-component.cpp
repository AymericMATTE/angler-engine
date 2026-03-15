#include "pch.h"
#include "state-machine-component.h"

namespace angler {
	void StateMachineComponent::addState(const std::string& _name, StateMachine<GameObject>::State _state) {
		m_stateMachine.setObject(getOwner());
		m_stateMachine.addState(_name, _state);
	}

	void StateMachineComponent::addState(std::string&& _name, StateMachine<GameObject>::State _state ) {
		m_stateMachine.setObject(getOwner());
		m_stateMachine.addState(_name, _state);
	}

	void StateMachineComponent::setOnBeginState(const std::string& _name, StateMachine<GameObject>::OnBegin _onBegin) {
		m_stateMachine.setOnBeginState(_name, _onBegin);
	}

	void StateMachineComponent::setOnBeginState(std::string&& _name, StateMachine<GameObject>::OnBegin _onBegin) {
		m_stateMachine.setOnBeginState(_name, _onBegin);
	}

	void StateMachineComponent::setOnUpdateState(const std::string& _name, StateMachine<GameObject>::OnUpdate _onUpdate) {
		m_stateMachine.setOnUpdateState(_name, _onUpdate);
	}

	void StateMachineComponent::setOnUpdateState(std::string&& _name, StateMachine<GameObject>::OnUpdate _onUpdate) {
		m_stateMachine.setOnUpdateState(_name, _onUpdate);
	}

	void StateMachineComponent::setOnEndState(const std::string& _name, StateMachine<GameObject>::OnEnd _onEnd) {
		m_stateMachine.setOnEndState(_name, _onEnd);
	}

	void StateMachineComponent::setOnEndState(std::string&& _name, StateMachine<GameObject>::OnEnd _onEnd) {
		m_stateMachine.setOnEndState(_name, _onEnd);
	}

	void StateMachineComponent::addTransition(const  StateMachine<GameObject>::Transition& _transition) {
		m_stateMachine.addTransition(_transition);
	}

	void StateMachineComponent::addTransition(StateMachine<GameObject>::Transition&& _transition) {
		m_stateMachine.addTransition(_transition);
	}

	void StateMachineComponent::OnUpdate() {
		m_stateMachine.setObject(getOwner());
		m_stateMachine.update();
	}

	void StateMachineComponent::transit(const std::string& _target) {
		m_stateMachine.setObject(getOwner());
		m_stateMachine.transit(_target);
	}

	void StateMachineComponent::transit(std::string&& _target) {
		m_stateMachine.setObject(getOwner());
		m_stateMachine.transit(_target);
	}

	std::string StateMachineComponent::getActualState() const {
		return m_stateMachine.getActualState();
	}
}