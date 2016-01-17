// =============================================================================
// File Name: GearBoxCANTalon.inl
// Description: Represents a gear box with up to 3 motors and an encoder
// Author: FRC Team 3512, Spartatroniks
// =============================================================================

#include <Solenoid.h>

inline GearBox<CANTalon>::GearBox(int shifterChan,
                                  int motor1,
                                  int motor2,
                                  int motor3) :
    GearBoxBase(shifterChan, -1, -1, motor1, motor2, motor3) {
    for (unsigned int i = 0; i < m_motors.size(); i++) {
        if (i == 0) {
            m_motors[i]->SetControlMode(CANTalon::kPercentVbus);
            m_motors[i]->SetFeedbackDevice(CANTalon::QuadEncoder);
            m_motors[i]->ConfigEncoderCodesPerRev(1);
            m_motors[i]->SetSensorDirection(m_isEncoderReversed);
            resetEncoder();
            setProfile(false);
            m_motors[i]->EnableControl();
        }
        else {
            // Use all other CANTalons as slaves
            m_motors[i]->SetControlMode(CANTalon::kFollower);

            // Set first CANTalon as master
            m_motors[i]->Set(motor1);
        }
    }
}

inline void GearBox<CANTalon>::setSetpoint(float setpoint) {
    m_motors[0]->SetControlMode(CANTalon::kPosition);

    m_setpoint = setpoint / m_distancePerPulse;
    m_motors[0]->Set(m_setpoint);
}

inline float GearBox<CANTalon>::getSetpoint() const {
    return m_setpoint * m_distancePerPulse;
}

inline void GearBox<CANTalon>::setManual(float value) {
    m_motors[0]->SetControlMode(CANTalon::kPercentVbus);
    m_motors[0]->Set(value);
}

inline float GearBox<CANTalon>::get(Grbx::PIDMode mode) const {
    if (mode == Grbx::Position) {
        return m_motors[0]->GetPosition() * m_distancePerPulse;
    }
    else if (mode == Grbx::Speed) {
        return m_motors[0]->GetEncVel() * m_distancePerPulse;
    }
    else if (mode == Grbx::Raw) {
        return m_motors[0]->Get();
    }

    return 0.f;
}

inline void GearBox<CANTalon>::setPID(float p, float i, float d) {
    m_motors[0]->SetPID(p, i, d);
}

inline void GearBox<CANTalon>::setF(float f) {
    m_motors[0]->SetF(f);
}

inline void GearBox<CANTalon>::setDistancePerPulse(double distancePerPulse) {
    m_distancePerPulse = distancePerPulse;
}

inline void GearBox<CANTalon>::resetEncoder() {
    m_motors[0]->SetPosition(0);
}

inline void GearBox<CANTalon>::setEncoderReversed(bool reverse) {
    m_isEncoderReversed = reverse;
    m_motors[0]->SetSensorDirection(m_isEncoderReversed);
}

inline bool GearBox<CANTalon>::onTarget() const {
    return abs(m_motors[0]->GetClosedLoopError()) < 15;
}

inline void GearBox<CANTalon>::resetPID() {
    m_motors[0]->ClearIaccum();
}

inline void GearBox<CANTalon>::setControlMode(CANTalon::ControlMode ctrlMode) {
    m_motors[0]->SetControlMode(ctrlMode);
}

inline void GearBox<CANTalon>::setSoftPositionLimits(double forwardLimit,
                                                     double backwardLimit) {
    m_motors[0]->ConfigSoftPositionLimits(forwardLimit, backwardLimit);
}

inline bool GearBox<CANTalon>::isFwdLimitSwitchClosed() const {
    return m_motors[0]->IsFwdLimitSwitchClosed();
}

inline bool GearBox<CANTalon>::isRevLimitSwitchClosed() const {
    return m_motors[0]->IsRevLimitSwitchClosed();
}

inline void GearBox<CANTalon>::setIZone(unsigned int value) {
    m_motors[0]->SetIzone(value);
}

inline void GearBox<CANTalon>::setCloseLoopRampRate(double value) {
    m_motors[0]->SetCloseLoopRampRate(value);
}

inline void GearBox<CANTalon>::setProfile(bool secondProfile) {
    m_motors[0]->SelectProfileSlot(secondProfile);
}

