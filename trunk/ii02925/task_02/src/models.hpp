#pragma once

#include <string>

struct LinearParameters { double a1, a2, a3, b1; };
struct NonlinearParameters { double a, b, c, d; };
struct DifferentialParameters { double a, b, dt; };

class Model {
public:
    virtual ~Model() = default;
    virtual double next(double u, double previousU = 0.0) = 0;
    virtual void reset() = 0;
};

class LinearModel19 final : public Model {
public:
    explicit LinearModel19(LinearParameters parameters = {0.5, 0.2, 0.1, 0.4});
    double next(double u, double previousU = 0.0) override;
    void reset() override;
    LinearParameters parameters() const;
private:
    LinearParameters p_;
    double y0_ = 0.0, y1_ = 0.0, y2_ = 0.0;
};

class NonlinearModel21 final : public Model {
public:
    explicit NonlinearModel21(NonlinearParameters parameters = {0.5, 0.05, 0.4, 0.1});
    double next(double u, double previousU = 0.0) override;
    void reset() override;
private:
    NonlinearParameters p_;
    double y0_ = 0.0, y1_ = 0.0;
};

class DifferentialModel33 final : public Model {
public:
    explicit DifferentialModel33(DifferentialParameters parameters = {0.6, 1.0, 0.1});
    double next(double u, double previousU = 0.0) override;
    void reset() override;
private:
    DifferentialParameters p_;
    double y_ = 0.0;
};

enum class SignalType { Step, Impulse, Harmonic };
double signalValue(SignalType type, int step, double amplitude = 1.0);
std::string signalName(SignalType type);
