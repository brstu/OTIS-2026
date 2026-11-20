#pragma once

#include <memory>
#include <string>

/// Типы входных воздействий, требуемые заданием.
enum class SignalType {
    Step,      ///< Ступенчатое: u_t = A
    Impulse,   ///< Импульсное: u_0 = A, u_t = 0 при t > 0
    Harmonic   ///< Гармоническое: u_t = A * sin(omega * t)
};

/// Абстрактное входное воздействие (паттерн "Стратегия").
class InputSignal {
public:
    virtual ~InputSignal() = default;

    /// Значение сигнала в момент tau.
    virtual double value(int tau) const = 0;

    /// Короткое имя для консоли: "step", "impulse", "harmonic".
    virtual std::string name() const = 0;

    /// Формула для отчёта.
    virtual std::string formula() const = 0;
};

class StepSignal : public InputSignal {
public:
    explicit StepSignal(double amplitude = 1.0);
    double value(int tau) const override;
    std::string name() const override;
    std::string formula() const override;

private:
    double amplitude_;
};

class ImpulseSignal : public InputSignal {
public:
    explicit ImpulseSignal(double amplitude = 1.0);
    double value(int tau) const override;
    std::string name() const override;
    std::string formula() const override;

private:
    double amplitude_;
};

class HarmonicSignal : public InputSignal {
public:
    explicit HarmonicSignal(double amplitude = 1.0, double omega = 1.0);
    double value(int tau) const override;
    std::string name() const override;
    std::string formula() const override;

private:
    double amplitude_;
    double omega_;
};

/// Разбор имени сигнала из командной строки.
SignalType parseSignalType(const std::string& text);

/// Фабрика входных воздействий.
std::unique_ptr<InputSignal> makeSignal(SignalType type,
                                        double amplitude = 1.0,
                                        double omega = 1.0);
