#pragma once

#include "engine/ecs/component-base.h"
#include "common/state-machine.h"

namespace angler {
   struct StateMachineComponent: public ComponentBase {
		void addState(const std::string& _name, StateMachine<ECS::Entity>::State _state = {});
		void addState(std::string&& _name, StateMachine<ECS::Entity>::State _state = {});

		void setOnBeginState(const std::string& _name, StateMachine<ECS::Entity>::OnBegin _onBegin);
		void setOnBeginState(std::string&& _name, StateMachine<ECS::Entity>::OnBegin _onBegin);
		void setOnUpdateState(const std::string& _name, StateMachine<ECS::Entity>::OnUpdate _onUpdate);
		void setOnUpdateState(std::string&& _name, StateMachine<ECS::Entity>::OnUpdate _onUpdate);
		void setOnEndState(const std::string& _name, StateMachine<ECS::Entity>::OnEnd _onEnd);
		void setOnEndState(std::string&& _name, StateMachine<ECS::Entity>::OnEnd _onEnd);

		void addTransition(const  StateMachine<ECS::Entity>::Transition& _transition);
		void addTransition(StateMachine<ECS::Entity>::Transition&& _transition);

		void update();

		void transit(const std::string& _target);
		void transit(std::string&& _target);

		[[nodiscard]] std::string getActualState() const;

    protected:
		StateMachine<ECS::Entity> m_stateMachine = {};
    }; 
}