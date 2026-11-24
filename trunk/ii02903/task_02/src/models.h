#pragma once
// Variant 3: Model 1.3 (linear), Model 2.5 (non-linear), Model 3.7 (ODE, Euler method).
#include <string>

// Abstract base class of all models.
class Model {
public:
    virtual ~Model() = default;

    // Computes y_{tau+1} from the input u_tau and moves the model one step forward.
    virtual double nextStep(double u) = 0;

    // Current output y_tau.
    virtual double current() const = 0;

    virtual std::string name() const = 0;
    virtual std::string tag() const = 0;   // short name for file names, e.g. "model13"
    virtual std::string equation() const = 0;
    virtual std::string params() const = 0;

    // Time step: 1 for discrete models, dt for differential equations.
    virtual double timeStep() const { return 1.0; }

    // Stability check. Returns false if the process is not stable.
    // 'info' receives a human-readable description (roots etc.). Default: nothing to check.
    virtual bool isStable(std::string& info) const;
};

// Model 1.3: y(tau+1) = a1*y(tau) + a2*y(tau-1) + b*u(tau)
class LinearModel13 : public Model {
public:
    LinearModel13(double a1, double a2, double b, double y0);

    double nextStep(double u) override;
    double current() const override { return yCur_; }

    std::string name() const override;
    std::string tag() const override { return "model13"; }
    std::string equation() const override;
    std::string params() const override;

    // Uses StabilityAnalyzer (roots of z^2 - a1*z - a2 = 0).
    bool isStable(std::string& info) const override;

private:
    double a1_;
    double a2_;
    double b_;
    double yCur_;         // y(tau)
    double yPrev_ = 0.0;  // y(tau-1), initial value 0
};

// Model 2.5: y(tau+1) = a*y(tau) + b*sign(u(tau))*(1 - exp(-|u(tau)|))
class NonlinearModel25 : public Model {
public:
    NonlinearModel25(double a, double b, double y0);

    double nextStep(double u) override;
    double current() const override { return y_; }

    std::string name() const override;
    std::string tag() const override { return "model25"; }
    std::string equation() const override;
    std::string params() const override;

private:
    // sign(u): +1, 0 or -1
    static double signOf(double u);

    double a_;
    double b_;
    double y_;
};

// Model 3.7: dy/dt = -exp(a*y) + b*u, Euler method:
//   y(k+1) = y(k) + dt * ( -exp(a*y(k)) + b*u(k) )
class DiffModel37 : public Model {
public:
    DiffModel37(double a, double b, double dt, double y0);

    double nextStep(double u) override;
    double current() const override { return y_; }
    double timeStep() const override { return dt_; }

    std::string name() const override;
    std::string tag() const override { return "model37"; }
    std::string equation() const override;
    std::string params() const override;

private:
    double a_;
    double b_;
    double dt_;
    double y_;
};
