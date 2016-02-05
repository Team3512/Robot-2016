// =============================================================================
// Description: Helps user determine if joystick button was just pressed or just
//             released
// Author: FRC Team 3512, Spartatroniks
// =============================================================================

#include <DriverStation.h>
#include "ButtonTracker.hpp"

ButtonTracker::ButtonTracker(uint32_t port) {
    m_port = port;
}

void ButtonTracker::UpdateButtons() {
    // "new" values are now "old"
    m_oldStates = m_newStates;

    // get newer values
    m_newStates = DriverStation::GetInstance().GetStickButtons(m_port);
}

bool ButtonTracker::PressedButton(uint32_t button) {
    return GetButtonState(m_oldStates, button) == false && // if button wasn't pressed
           GetButtonState(m_newStates, button) == true; // and it is now
}

bool ButtonTracker::ReleasedButton(uint32_t button) {
    return GetButtonState(m_oldStates, button) == true && // if button was pressed
           GetButtonState(m_newStates, button) == false; // and it isn't now
}

bool ButtonTracker::GetButtonState(short& buttonStates, uint32_t& button) {
    return ((1 << (button - 1)) & buttonStates) != 0;
}
