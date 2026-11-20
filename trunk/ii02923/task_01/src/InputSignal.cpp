#include "InputSignal.h"

#include <cmath>
#include <stdexcept>

StepSignal::StepSignal(double amplitude) : amplitude_(amplitude) {}

double StepSignal::value(int) const {
    return amplitude_;
}

std::string StepSignal::name() const {
    return "step";
}

std::string StepSignal::formula() const {
    return "u[t] = A (ступенчатое)";
}

ImpulseSignal::ImpulseSignal(double amplitude) : amplitude_(amplitude) {}

double ImpulseSignal::value(int tau) const {
    return tau == 0 ? amplitude_ : 0.0;
}

std::string ImpulseSignal::name() const {
    return "impulse";
}

std::string ImpulseSignal::formula() const {
    return "u[0] = A, u[t>0] = 0 (импульсное)";
}

HarmonicSignal::HarmonicSignal(double amplitude, double omega)
    : amplitude_(amplitude), omega_(omega) {}

double HarmonicSignal::value(int tau) const {
    return amplitude_ * std::sin(omega_ * static_cast<double>(tau));
}

std::string HarmonicSignal::name() const {
    return "harmonic";
}

std::string HarmonicSignal::formula() const {
    return "u[t] = A*sin(omega*t) (гармоническое)";
}

SignalType parseSignalType(const std::string& text) {
    if (text == "step" || text == "ступенчатое") {
        return SignalType::Step;
    }
    if (text == "impulse" || text == "импульсное") {
        return SignalType::Impulse;
    }
    if (text == "harmonic" || text == "гармоническое") {
        return SignalType::Harmonic;
    }
    throw std::invalid_argument("Неизвестный тип сигнала: " + text +
                                " (ожидается step, impulse или harmonic)");
}

std::unique_ptr<InputSignal> makeSignal(SignalType type, double amplitude,
                                        double omega) {
    switch (type) {
        case SignalType::Step:
            return std::make_unique<StepSignal>(amplitude);
        case SignalType::Impulse:
            return std::make_unique<ImpulseSignal>(amplitude);
        case SignalType::Harmonic:
            return std::make_unique<HarmonicSignal>(amplitude, omega);
    }
    return std::make_unique<StepSignal>(amplitude);
}
