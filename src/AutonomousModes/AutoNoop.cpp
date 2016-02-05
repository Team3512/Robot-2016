// =============================================================================
// Description: Does nothing
// Author: FRC Team 3512, Spartatroniks
// =============================================================================

#include "../Robot.hpp"

void Robot::AutoNoop() {
    while (IsAutonomous() && IsEnabled()) {
        DS_PrintOut();

        std::this_thread::sleep_for(10ms);
    }
}
