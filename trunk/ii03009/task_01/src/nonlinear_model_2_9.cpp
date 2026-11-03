#include "nonlinear_model_2_9.h"
#include <cmath>

NonlinearModel2_9::NonlinearModel2_9(double a, double b)
    : a_(a), b_(b), y_(0.0) {}

double NonlinearModel2_9::step(double u) {
    double sign_u = (u > 0) - (u < 0); 
    double u_mod = std::sqrt(std::abs(u)) * sign_u;
    
    y_ = a_ * y_ + b_ * u_mod;
    return y_;
}

void NonlinearModel2_9::reset() {
    y_ = 0.0;
}