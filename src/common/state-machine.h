#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace angler
{
	template<typename T>
	struct StateMachine
	{
		using OnBegin = void(*)(T* _object);
		using OnUpdate = void(*)(T* _object);
		using OnEnd = void(*)(T* _object);
		using Condition = bool(*)(T* _object);

		struct Transition
		{
			Transition(const std::vector<Condition>& _conditions, const std::string& _target) : conditions(_conditions), target(_target) {}
			Transition(std::vector<Condition>&& _conditions, std::string&& _target) : conditions(_conditions), target(_target) {}

			std::vector<Condition> conditions;
			std::string target;
		};

		struct State
		{
			State() = default;
			State(OnBegin _onBegin, OnUpdate _onUpdate, OnEnd _onEnd) : onBegin(_onBegin), onUpdate(_onUpdate), onEnd(_onEnd) {}

			OnBegin onBegin = nullptr;
			OnUpdate onUpdate = nullptr;
			OnEnd onEnd = nullptr;
		};

		StateMachine() = default;
		StateMachine(T* _object);

		void setObject(T* _object);

		void addState(const std::string& _name, State _state = {});
		void addState(std::string&& _name, State _state = {});

		void setOnBeginState(const std::string& _name, OnBegin _onBegin);
		void setOnBeginState(std::string&& _name, OnBegin _onBegin);
		void setOnUpdateState(const std::string& _name, OnUpdate _onUpdate);
		void setOnUpdateState(std::string&& _name, OnUpdate _onUpdate);
		void setOnEndState(const std::string& _name, OnEnd _onEnd);
		void setOnEndState(std::string&& _name, OnEnd _onEnd);

		void addTransition(const Transition& _transition);
		void addTransition(Transition&& _transition);

		void update();

		void transit(const std::string& _target);
		void transit(std::string&& _target);

		[[nodiscard]] std::string getActualState() const;

	private:

		T* m_object = nullptr;

		std::string m_actualState = {};

		std::unordered_map<std::string, State> m_states = {};
		std::vector<Transition> m_transitions = {};
	};
}

#include "state-machine.inl"