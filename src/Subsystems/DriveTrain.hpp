// =============================================================================
// File Name: DriveTrain.hpp
// Description: Provides an interface for this year's drive train
// Author: FRC Team 3512, Spartatroniks
// =============================================================================

#ifndef DRIVE_TRAIN_HPP
#define DRIVE_TRAIN_HPP

#include "SubsystemBase.hpp"
#include "../MotionProfile/BezierTrapezoidProfile.hpp"
#include "GearBox.hpp"

#include <CANTalon.h>

class DriveTrain : public SubsystemBase, public BezierTrapezoidProfile {
public:
    DriveTrain();

    /* Drives robot with given speed and turn values [-1..1].
     * This is a convenience function for use in Operator Control.
     */
    void drive(float throttle, float turn, bool isQuickTurn = false);

    // Sets joystick deadband
    void setDeadband(float band);

    // Reload PID constants
    void reloadPID();

    // Set encoder distances to 0
    void resetEncoders();

    // Set wheel setpoints (see GearBox::setSetpoint(float))
    void setLeftSetpoint(double setpt);
    void setRightSetpoint(double setpt);

    // Directly set wheel speeds [0..1] (see GearBox::setManual(float))
    void setLeftManual(float value);
    void setRightManual(float value);

    // Returns encoder distances
    double getLeftDist() const;
    double getRightDist() const;

    // Returns encoder rates
    double getLeftRate() const;
    double getRightRate() const;

    // Returns encoder PID loop setpoints
    double getLeftSetpoint() const;
    double getRightSetpoint() const;

    void setControlMode(CANTalon::ControlMode ctrlMode);

    const static float maxWheelSpeed;

private:
    float m_deadband = 0.02f;
    float m_sensitivity;

    // Cheesy Drive variables
    float m_oldTurn = 0.f;
    float m_quickStopAccumulator = 0.f;
    float m_negInertiaAccumulator = 0.f;

    GearBox<CANTalon> m_leftGrbx{-1, 4, 1};
    GearBox<CANTalon> m_rightGrbx{-1, 5, 8};

    /* Zeroes value if its inside deadband range, and rescales values outside
     * of it
     */
    float applyDeadband(float value);

    // Limits 'value' to within +- 'limit' (limit should be positive)
    template <class T>
    T limit(T value, T limit);
};

#include "DriveTrain.inl"

#endif // DRIVE_TRAIN_HPP

