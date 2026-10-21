#pragma once
#include "model.hpp"
#include <vector>

namespace otis {

// Model 1.2: y[t+1] = a*y[t] + b*u[t-k]
class DelayedControlModel final : public Model {
public:
    DelayedControlModel(double a, double b, int delay);
    double step(double input, int tau) override;
    void reset() override;
    [[nodiscard]] const char* name() const override;
private:
    double a_, b_, y_ = 0.0;
    int delay_;
    std::vector<double> inputs_;
};

// Model 2.4: y[t+1] = a*y[t]*cos(y[t]) + b*ln(1 + |u[t]|)
class LogarithmicInteractionModel final : public Model {
public:
    LogarithmicInteractionModel(double a, double b);
    double step(double input, int tau) override;
    void reset() override;
    [[nodiscard]] const char* name() const override;
private:
    double a_, b_, y_ = 0.0;
};

// Model 3.6: dy/dt = a*y^3 + b*u, solved with explicit Euler.
class CubicDifferentialModel final : public Model {
public:
    CubicDifferentialModel(double a, double b, double dt);
    double step(double input, int tau) override;
    void reset() override;
    [[nodiscard]] const char* name() const override;
private:
    double a_, b_, dt_, y_ = 0.0;
};

} // namespace otis
