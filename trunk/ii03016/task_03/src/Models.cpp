#include "../../task_01/src/Model 1.10.h"
#include "../../task_01/src/Model 2.5.h"
#include "../../task_01/src/Model 3.10.h"
#include <cmath>

// --- Модель 1.10 ---
LineModel_1_10::LineModel_1_10(double a1_, double b1_, double b2_)
    : a1(a1_), b1(b1_), b2(b2_) {}

void LineModel_1_10::reset() {
    y = 0.0;
    u_prev1 = 0.0;
    u_prev2 = 0.0;
}

double LineModel_1_10::step(double u) {
    double y_new = a1 * y + b1 * u + b2 * u_prev2;
    u_prev2 = u_prev1;
    u_prev1 = u;
    y = y_new;
    return y;
}

const char* LineModel_1_10::name() { return "Linear Model 1.10"; }

// --- Модель 2.5 ---
namespace {
double sign(double x) {
    if (x > 0) return 1.0;
    if (x < 0) return -1.0;
    return 0.0;
}
} // namespace

NonlinearModel_2_5::NonlinearModel_2_5(double a_, double b_) : a(a_), b(b_) {}

void NonlinearModel_2_5::reset() { y = 0.0; }

double NonlinearModel_2_5::step(double u) {
    y = a * y + b * sign(u) * (1.0 - std::exp(-std::fabs(u)));
    return y;
}

const char* NonlinearModel_2_5::name() { return "Nonlinear Model 2.5"; }

// --- Модель 3.10 ---
Diff_3_10::Diff_3_10(double a_, double b_, double dt_) : a(a_), b(b_), dt(dt_) {}

void Diff_3_10::reset() { y = 0.0; }

double Diff_3_10::step(double u) {
    y = y + dt * (-a * y + b + u);
    return y;
}

const char* Diff_3_10::name() { return "Diff 3.10"; }
