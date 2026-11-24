#include "models.h"

#include <cmath>

#include "signals.h"
#include "stability.h"

// ---------------------------------------------------------------------------
// Model (base class)
// ---------------------------------------------------------------------------
bool Model::isStable(std::string& info) const {
    info.clear();
    return true;
}

// ---------------------------------------------------------------------------
// Model 1.3
// ---------------------------------------------------------------------------
LinearModel13::LinearModel13(double a1, double a2, double b, double y0)
    : a1_(a1), a2_(a2), b_(b), yCur_(y0) {}

double LinearModel13::nextStep(double u) {
    const double next = a1_ * yCur_ + a2_ * yPrev_ + b_ * u;
    yPrev_ = yCur_;
    yCur_ = next;
    return yCur_;
}

std::string LinearModel13::name() const { return "Model 1.3 (second-order linear)"; }

std::string LinearModel13::equation() const {
    return "y(t+1) = a1*y(t) + a2*y(t-1) + b*u(t)";
}

std::string LinearModel13::params() const {
    return "a1=" + num(a1_) + ", a2=" + num(a2_) + ", b=" + num(b_);
}

bool LinearModel13::isStable(std::string& info) const {
    const StabilityResult r = StabilityAnalyzer::analyze(a1_, a2_);
    info = StabilityAnalyzer::describe(r);
    return r.stable;
}

// ---------------------------------------------------------------------------
// Model 2.5
// ---------------------------------------------------------------------------
NonlinearModel25::NonlinearModel25(double a, double b, double y0) : a_(a), b_(b), y_(y0) {}

double NonlinearModel25::nextStep(double u) {
    y_ = a_ * y_ + b_ * signOf(u) * (1.0 - std::exp(-std::fabs(u)));
    return y_;
}

std::string NonlinearModel25::name() const { return "Model 2.5 (signum friction, non-linear)"; }

std::string NonlinearModel25::equation() const {
    return "y(t+1) = a*y(t) + b*sign(u(t))*(1 - exp(-|u(t)|))";
}

std::string NonlinearModel25::params() const { return "a=" + num(a_) + ", b=" + num(b_); }

double NonlinearModel25::signOf(double u) {
    if (u > 0.0) return 1.0;
    if (u < 0.0) return -1.0;
    return 0.0;
}

// ---------------------------------------------------------------------------
// Model 3.7
// ---------------------------------------------------------------------------
DiffModel37::DiffModel37(double a, double b, double dt, double y0)
    : a_(a), b_(b), dt_(dt), y_(y0) {}

double DiffModel37::nextStep(double u) {
    y_ = y_ + dt_ * (-std::exp(a_ * y_) + b_ * u);
    return y_;
}

std::string DiffModel37::name() const { return "Model 3.7 (ODE, Euler method)"; }

std::string DiffModel37::equation() const { return "dy/dt = -exp(a*y) + b*u"; }

std::string DiffModel37::params() const {
    return "a=" + num(a_) + ", b=" + num(b_) + ", dt=" + num(dt_);
}
