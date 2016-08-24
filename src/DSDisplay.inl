// Copyright (c) FRC Team 3512, Spartatroniks 2016. All Rights Reserved.

template <class T>
void DSDisplay::AddAutoMethod(const std::string& methodName,
                              void (T::*function)(), T* object) {
    m_autonModes.AddMethod(methodName, std::bind(function, object));
}
