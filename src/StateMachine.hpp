// =============================================================================
// File Name: StateMachine.hpp
// Description: Provides an easier way to create state machines
// Author: FRC Team 3512, Spartatroniks
// =============================================================================

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <memory>
#include <vector>
#include "State.hpp"

/* The default state is "IDLE". States that end the state machine should return
 * "IDLE" from advanceFunc()
 */

class StateMachine {
public:
    /* Ownership of 'state' will be transferred to this class, which will handle
     * destroying it.
     */
    void addState(State* state);
    void addState(std::unique_ptr<State> state);

    template <class T,
              class =
                  std::enable_if_t<!std::is_same<std::decay_t<T>,
                                                 State>::value>>
    void emplaceState(T&& state) noexcept(std::is_nothrow_assignable<State&,
                                                                     T>::value)
    {
        m_states.push_back(std::make_unique<State>(std::forward<T>(state)));
    }

    /* Moves the state machine to the given state. If the next state is found,
     * endFunc() for the current state and initFunc() for the next state. 'true'
     * is returned if the next state was found and 'false' otherwise.
     */
    bool setState(const std::string& nextState);

    // Returns name of current state
    const std::string getState() const;

    // Call this periodically to operate the state machine
    void run();

private:
    std::vector<std::unique_ptr<State>> m_states;
    State* m_currentState = nullptr;
};

#endif // STATE_MACHINE_HPP

