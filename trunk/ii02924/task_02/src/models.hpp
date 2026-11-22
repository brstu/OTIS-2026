#pragma once

#include <string>

struct LinearParameters {
    double a1;
    double a2;
    double a3;
    double b1;
};
struct NonlinearParameters {
    double a;
    double b;
    double c;
    double d;
};
struct DifferentialParameters {
    double a;
    double b;
    double dt;
};

class Model {
public:
    virtual ~Model() = default;
    virtual double next(double u, double previousU = 0.0) = 0;
    virtual void reset() = 0;
};

class LinearModel19 final : public Model {
public:
    explicit LinearModel19(const LinearParameters& parameters = {0.5, 0.2, 0.1, 0.4});
    double next(double u, double previousU = 0.0) override;
    void reset() override;
    LinearParameters parameters() const;
private:
    LinearParameters p_;
    double y0_ = 0.0;
    double y1_ = 0.0;
    double y2_ = 0.0;
};

class NonlinearModel21 final : public Model {
public:
    explicit NonlinearModel21(const NonlinearParameters& parameters = {0.5, 0.05, 0.4, 0.1});
    double next(double u, double previousU = 0.0) override;
    void reset() override;
private:
    NonlinearParameters p_;
    double y0_ = 0.0;
    double y1_ = 0.0;
};

class DifferentialModel33 final : public Model {
public:
    explicit DifferentialModel33(const DifferentialParameters& parameters = {0.6, 1.0, 0.1});
    double next(double u, double previousU = 0.0) override;
    void reset() override;
private:
    DifferentialParameters p_;
    double y_ = 0.0;
};

enum class SignalType { Step, Impulse, Harmonic };
double signalValue(SignalType type, int step, double amplitude = 1.0);
std::string signalName(SignalType type);
