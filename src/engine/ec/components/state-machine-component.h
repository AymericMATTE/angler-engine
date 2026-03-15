#pragma once

#include "engine/ec/component-base.h"
#include "common/state-machine.h"

namespace angler {
   struct StateMachineComponent: public ComponentBase<StateMachineComponent> {
		void addState(const std::string& _name, StateMachine<GameObject>::State _state = {});
		void addState(std::string&& _name, StateMachine<GameObject>::State _state = {});

		void setOnBeginState(const std::string& _name, StateMachine<GameObject>::OnBegin _onBegin);
		void setOnBeginState(std::string&& _name, StateMachine<GameObject>::OnBegin _onBegin);
		void setOnUpdateState(const std::string& _name, StateMachine<GameObject>::OnUpdate _onUpdate);
		void setOnUpdateState(std::string&& _name, StateMachine<GameObject>::OnUpdate _onUpdate);
		void setOnEndState(const std::string& _name, StateMachine<GameObject>::OnEnd _onEnd);
		void setOnEndState(std::string&& _name, StateMachine<GameObject>::OnEnd _onEnd);

		void addTransition(const  StateMachine<GameObject>::Transition& _transition);
		void addTransition(StateMachine<GameObject>::Transition&& _transition);

		void OnUpdate();

		void transit(const std::string& _target);
		void transit(std::string&& _target);

		[[nodiscard]] std::string getActualState() const;

    protected:
		StateMachine<GameObject> m_stateMachine = {};
    }; 
}