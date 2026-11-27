#ifndef OTIS_II02926_TASK02_INPUT_SIGNALS_H
#define OTIS_II02926_TASK02_INPUT_SIGNALS_H

#include <cmath>
#include <string>

class InputSignal {
public:
    using StepIndex = int;
    using SignalValue = double;

    virtual ~InputSignal() noexcept = default;

    virtual SignalValue value(StepIndex step) const = 0;
    virtual std::string name() const = 0;
};

// Ступенчатое: u_tau = const
class StepInput final : public InputSignal {
public:
    static constexpr SignalValue DEFAULT_AMPLITUDE = 1.0;

    explicit StepInput(SignalValue amp = DEFAULT_AMPLITUDE) noexcept;

    SignalValue value(StepIndex step) const override;
    std::string name() const override;

private:
    SignalValue amplitude;
};

// Импульсное: u_1 = 1, u_{tau>1} = 0
class ImpulseInput final : public InputSignal {
public:
    SignalValue value(StepIndex step) const override;
    std::string name() const override;

private:
    static constexpr StepIndex FIRST_STEP = 1;
    static constexpr SignalValue UNIT_VALUE = 1.0;
    static constexpr SignalValue ZERO_VALUE = 0.0;
};

// Гармоническое: u_tau = sin(tau)
class HarmonicInput final : public InputSignal {
public:
    SignalValue value(StepIndex step) const override;
    std::string name() const override;
};

inline StepInput::StepInput(SignalValue amp) noexcept
    : amplitude(amp) {}

inline InputSignal::SignalValue StepInput::value(StepIndex /*step*/) const {
    return amplitude;
}

inline std::string StepInput::name() const {
    const std::string prefix = "Step (u = const = ";
    return prefix + std::to_string(amplitude) + ")";
}

inline InputSignal::SignalValue ImpulseInput::value(StepIndex step) const {
    if (step == FIRST_STEP) {
        return UNIT_VALUE;
    }
    return ZERO_VALUE;
}

inline std::string ImpulseInput::name() const {
    return "Impulse (u_1 = 1, u_tau>1 = 0)";
}

inline InputSignal::SignalValue HarmonicInput::value(StepIndex step) const {
    const auto angle = static_cast<double>(step);
    return std::sin(angle);
}

inline std::string HarmonicInput::name() const {
    return "Harmonic (u_tau = sin(tau))";
}

#endif // OTIS_II02926_TASK02_INPUT_SIGNALS_H
