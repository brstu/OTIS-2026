#include "models.h"
#include "stability.h"
#include <cmath>
#include <sstream>

namespace {
std::string fmt(double v) {
    std::ostringstream oss;
    oss << v;
    return oss.str();
}
}

bool Model::isStable(std::string& info) const {
    info.clear();
    return true;
}

// ----- LinearModel13 -----
LinearModel13::LinearModel13(double a1, double a2, double b, double y0)
    : a1_(a1), a2_(a2), b_(b), y_now_(y0) {}

double LinearModel13::nextStep(double u) {
    double next = a1_ * y_now_ + a2_ * y_old_ + b_ * u;
    y_old_ = y_now_;
    y_now_ = next;
    return y_now_;
}

std::string LinearModel13::name() const {
    return "Model 1.3 linear second order";
}

std::string LinearModel13::equation() const {
    return "y(k+1) = a1*y(k) + a2*y(k-1) + b*u(k)";
}

std::string LinearModel13::params() const {
    return "a1=" + fmt(a1_) + " a2=" + fmt(a2_) + " b=" + fmt(b_);
}

bool LinearModel13::isStable(std::string& info) const {
    auto result = StabilityAnalyzer::analyze(a1_, a2_);
    info = StabilityAnalyzer::describe(result);
    return result.stable;
}

// ----- NonlinearModel25 -----
NonlinearModel25::NonlinearModel25(double a, double b, double y0)
    : a_(a), b_(b), state_(y0) {}

double NonlinearModel25::signum(double v) {
    if (v > 0.0) return 1.0;
    if (v < 0.0) return -1.0;
    return 0.0;
}

double NonlinearModel25::nextStep(double u) {
    state_ = a_ * state_ + b_ * signum(u) * (1.0 - std::exp(-std::fabs(u)));
    return state_;
}

std::string NonlinearModel25::name() const {
    return "Model 2.5 nonlinear sign friction";
}

std::string NonlinearModel25::equation() const {
    return "y(k+1) = a*y(k) + b*sign(u)*(1-exp(-|u|))";
}

std::string NonlinearModel25::params() const {
    return "a=" + fmt(a_) + " b=" + fmt(b_);
}

// ----- DiffModel37 -----
DiffModel37::DiffModel37(double a, double b, double dt, double y0)
    : a_(a), b_(b), step_(dt), state_(y0) {}

double DiffModel37::nextStep(double u) {
    state_ += step_ * (-std::exp(a_ * state_) + b_ * u);
    return state_;
}

std::string DiffModel37::name() const {
    return "Model 3.7 ODE Euler";
}

std::string DiffModel37::equation() const {
    return "dy/dt = -exp(a*y) + b*u";
}

std::string DiffModel37::params() const {
    return "a=" + fmt(a_) + " b=" + fmt(b_) + " dt=" + fmt(step_);
}
