// =============================================================================
// File Name: StateMachine.cpp
// Description: Provides an easier way to create state machines
// Author: FRC Team 3512, Spartatroniks
// =============================================================================

#include "StateMachine.hpp"
#include <iostream>

StateMachine::StateMachine(std::string name) : State(std::move(name)) {
    Run = [this] {
        if (m_currentState == nullptr) {
            return;
        }

        m_currentState->Run();

        std::string nextState = m_currentState->Transition();

        if (nextState.size() != 0) {
            if (!SetState(nextState)) {
                // Failed to find state matching the returned name
                std::cout << "[" << nextState << "] is not a known state\n";
            }
        }
    };
}

void StateMachine::AddState(std::unique_ptr<State> state) {
    m_states.push_back(std::move(state));
}

bool StateMachine::SetState(const std::string& newState) {
    for (auto& i : m_states) {
        if (i->Name() == newState) {
            if (m_currentState != nullptr) {
                m_currentState->Exit();
            }
            m_currentState = i.get();
            m_currentState->Entry();

            return true;
        }
    }

    return false;
}

const std::string StateMachine::GetState() const {
    if (m_currentState != nullptr) {
        return m_currentState->Name();
    }
    else {
        return "";
    }
}

