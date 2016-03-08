// =============================================================================
// Description: Provides an interface for the robot's arm
// Author: FRC Team 3512, Spartatroniks
// =============================================================================

#include "../WPILib/PIDController.hpp"
#include "Arm.hpp"
#include <iostream>

Arm::Arm() {
    m_leftArmPID = std::make_shared<PIDController>(0.f,
                                                   0.f,
                                                   0.f,
                                                   0.f,
                                                   0.f,
                                                   &m_leftArmActuator,
                                                   &m_leftArmActuator);
    m_leftArmProfile = std::make_shared<TrapezoidProfile>(m_leftArmPID, 0.0,
                                                          0.0);

    m_carriagePID = std::make_shared<PIDController>(0.f,
                                                    0.f,
                                                    0.f,
                                                    0.f,
                                                    0.f,
                                                    &m_carriageGrbx,
                                                    &m_carriageGrbx);
    m_carriageProfile = std::make_shared<TrapezoidProfile>(m_carriagePID, 0.0,
                                                           0.0);

    m_winchPID = std::make_shared<PIDController>(0.f,
                                                 0.f,
                                                 0.f,
                                                 0.f,
                                                 0.f,
                                                 &m_winchGrbx,
                                                 &m_winchGrbx);
    m_winchProfile = std::make_shared<TrapezoidProfile>(m_winchPID, 0.0, 0.0);

    m_leftCarriageLimit = DigitalInputHandler::Get(k_leftCarriageLimitChannel);

    // Sets encoder type
    m_leftArmActuator.GetMaster()->SetFeedbackDevice(
        CANTalon::CtreMagEncoder_Relative);
    m_leftArmActuator.SetSensorDirection(true);
    m_leftArmActuator.SetDistancePerPulse(30.0f / 133.0f); // Angle per pulse

    m_joystickEvent.RegisterButtonEvent("ZeroHeightButton", k_armStickPort,
                                        k_armZeroButton,
                                        true); // TODO: change port number
    m_joystickEvent.RegisterButtonEvent("CarryingHeightButton", k_armStickPort,
                                        k_armCarryingButton,
                                        true); // TODO: change port number
    m_dioEvent.RegisterInputEvent("LeftArmZeroed", k_armLeftBottomLimitChannel,
                                  true,
                                  false, m_armSM);
    m_dioEvent.RegisterInputEvent("LeftArmUp", k_armLeftTopLimitChannel,
                                  true,
                                  false, m_armSM);

    // Idle
    auto state = std::make_unique<State>("Idle");
    state->Entry = [this] {
        m_leftArmActuator.Set(0);
    };
    state->CheckTransition = [this] (const std::string& event) {
                                 if (event == "CarryingHeightButton") {
                                     m_leftArmProfile->SetGoal({k_armHeight1,
                                                                0.0, 0.0});
                                     return "CarryingHeight";
                                 }
                                 else if (event == "ZeroHeightButton") {
                                     return "ZeroHeight";
                                 }
                                 else {
                                     return "";
                                 }
                             };
    m_armSM.AddState(std::move(state));

    // Start arm lift
    state = std::make_unique<State>("CarryingHeight");
    state->Run = [this] {
        if (GetManualOverride()) {
            m_leftArmActuator.Set(1);
        }
    };
    state->CheckTransition = [this] (const std::string& event) {
                                 if (AtGoal()) {
                                     return "Idle";
                                 }
                                 else {
                                     return "";
                                 }
                             };

    m_armSM.AddState(std::move(state));

    // Start arm lift
    state = std::make_unique<State>("ZeroHeight");
    state->Run = [this] {
        m_leftArmActuator.Set(-1);
    };
    state->CheckTransition = [this] (const std::string& event) {
                                 if (event == "LeftArmZeroed") {
                                     m_leftArmActuator.ResetEncoder();
                                     return "Idle";
                                 }
                                 else {
                                     return "";
                                 }
                             };

    m_armSM.AddState(std::move(state));
}

void Arm::SetManualOverride(bool manual) {
    m_manual = manual;
}

bool Arm::GetManualOverride() const {
    return m_manual;
}

void Arm::SetCarryingHeight(double speed) {
    if (GetManualOverride()) {
        m_leftArmPID->Disable();
        m_armHeight = speed;
    }
    else {
        m_leftArmPID->Enable();
        m_leftArmPID->SetSetpoint({ 0.0, speed / m_leftArmPID->GetV(), 0.0 }); // TODO: change all of these values
    }
}

void Arm::SetArmHeight(double height) {
    if (GetManualOverride()) {
        m_leftArmActuator.Set(height);
    }
}

int32_t Arm::GetArmHeightValue() const {
    return m_leftArmActuator.Get();
}

double Arm::GetArmSpeed() const {
    return m_leftArmActuator.GetSpeed();
}
void Arm::SetManualCarriagePosition(int direction) {
    // TODO: Check the limit switches before setting the direction.
    m_carriageGrbx.Set(direction);
}
bool Arm::AtGoal() const {
    return m_leftArmProfile->AtGoal();
}

void Arm::ReloadPID() {
}

void Arm::SetManualWinchHeight(double speed) {
    m_winchGrbx.Set(speed);
}

void Arm::ResetEncoders() {
}

void Arm::UpdateState() {
    m_armSM.Run();
    m_joystickEvent.Poll(m_armSM);
    m_joystickEvent.Update();
}
