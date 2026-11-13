#ifndef INPUT_SIGNAL_H
#define INPUT_SIGNAL_H

// Источник входного воздействия u(tau). Интерфейс позволяет подменять сигнал
// (в том числе mock-объектом Google Mock) при тестировании режимов управления.
class ISignal {
public:
    virtual ~ISignal() = default;
    virtual double value(int tau) const = 0;
};

// Ступенчатое: u(tau) = A
class StepSignal : public ISignal {
    double amplitude_;
public:
    explicit StepSignal(double amplitude);
    double value(int tau) const override;
};

// Импульсное: u(0) = A, u(tau > 0) = 0
class PulseSignal : public ISignal {
    double amplitude_;
public:
    explicit PulseSignal(double amplitude);
    double value(int tau) const override;
};

// Гармоническое: u(tau) = A * sin(tau * phaseStep)
// (phaseStep = 1 даёт A*sin(tau); в ЛР №1 использовалось phaseStep = dt)
class HarmonicSignal : public ISignal {
    double amplitude_;
    double phaseStep_;
public:
    explicit HarmonicSignal(double amplitude, double phaseStep = 1.0);
    double value(int tau) const override;
};

#endif
