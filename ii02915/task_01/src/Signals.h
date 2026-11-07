#pragma once
#include <cmath>

// Типы входных воздействий по заданию
enum class SignalType {
    Step = 1,     // ступенчатое: u = A
    Impulse = 2,  // импульсное: u(0) = A, u(tau > 0) = 0
    Harmonic = 3  // гармоническое: u = A*sin(tau)
};

inline double getInput(SignalType type, int tau, double amplitude) {
    switch (type) {
        case SignalType::Step:
            return amplitude;
        case SignalType::Impulse:
            return (tau == 0) ? amplitude : 0.0;
        case SignalType::Harmonic:
            return amplitude * std::sin(static_cast<double>(tau));
    }
    return 0.0;
}

inline const char* signalName(SignalType type) {
    switch (type) {
        case SignalType::Step:
            return "step";
        case SignalType::Impulse:
            return "impulse";
        case SignalType::Harmonic:
            return "harmonic";
    }
    return "unknown";
}
