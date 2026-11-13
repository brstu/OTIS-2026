#include "models.hpp"
#include <cmath>

LinearModel19::LinearModel19(const LinearParameters& parameters) : p_(parameters) {}
double LinearModel19::next(double u, double) {
    const double result = p_.a1*y0_ + p_.a2*y1_ + p_.a3*y2_ + p_.b1*u;
    y2_ = y1_;
    y1_ = y0_;
    y0_ = result;
    return result;
}
void LinearModel19::reset() {
    y0_ = 0.0;
    y1_ = 0.0;
    y2_ = 0.0;
}
LinearParameters LinearModel19::parameters() const { return p_; }

NonlinearModel21::NonlinearModel21(const NonlinearParameters& parameters) : p_(parameters) {}
double NonlinearModel21::next(double u, double previousU) {
    const double result = p_.a*y0_ - p_.b*y1_*y1_ + p_.c*u + p_.d*std::sin(previousU);
    y1_ = y0_;
    y0_ = result;
    return result;
}
void NonlinearModel21::reset() {
    y0_ = 0.0;
    y1_ = 0.0;
}

DifferentialModel33::DifferentialModel33(const DifferentialParameters& parameters) : p_(parameters) {}
double DifferentialModel33::next(double u, double) {
    y_ += p_.dt * (-p_.a*y_ + p_.b*u);
    return y_;
}
void DifferentialModel33::reset() { y_ = 0.0; }

double signalValue(SignalType type, int step, double amplitude) {
    if (type == SignalType::Step) return amplitude;
    if (type == SignalType::Impulse) return step == 0 ? amplitude : 0.0;
    return amplitude * std::sin(static_cast<double>(step));
}
std::string signalName(SignalType type) {
    if (type == SignalType::Step) return "step";
    if (type == SignalType::Impulse) return "impulse";
    return "harmonic";
}
