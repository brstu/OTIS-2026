#pragma once
#include <cmath>

class InputGenerator {
public:
    virtual ~InputGenerator() = default;
    virtual double generate(int tau) const = 0;
};

class StepInput : public InputGenerator {
public:
    double generate(int tau) const override { return 1.0; }
};

class ImpulseInput : public InputGenerator {
public:
    double generate(int tau) const override { return (tau == 0) ? 1.0 : 0.0; }
};

class HarmonicInput : public InputGenerator {
public:
    double generate(int tau) const override { return std::sin(static_cast<double>(tau)); }
};