#pragma once
#include <cmath>
#include <sstream>
#include <string>

inline std::string to_str(double v) {
    std::ostringstream oss;
    oss << v;
    return oss.str();
}

class InputSignal {
public:
    virtual \~InputSignal() = default;
    virtual double value(int step, double time) const = 0;
    virtual std::string name() const = 0;
    virtual std::string tag() const = 0;
};

class StepInput : public InputSignal {
public:
    explicit StepInput(double level) : level_(level) {}
    double value(int, double) const override { return level_; }
    std::string name() const override { return "constant " + to_str(level_); }
    std::string tag() const override { return "step"; }
private:
    double level_;
};

class ImpulseInput : public InputSignal {
public:
    double value(int step, double) const override {
        return step == 0 ? 1.0 : 0.0;
    }
    std::string name() const override { return "unit impulse"; }
    std::string tag() const override { return "impulse"; }
};

class HarmonicInput : public InputSignal {
public:
    double value(int, double time) const override {
        return std::sin(time);
    }
    std::string name() const override { return "sinusoidal"; }
    std::string tag() const override { return "harmonic"; }
};
