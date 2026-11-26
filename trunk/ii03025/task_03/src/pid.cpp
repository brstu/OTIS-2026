#include "pid.h"

PIDController::PIDController(double p, double i, double d,
                             double min, double max)
    : kp(p), ki(i), kd(d), uMin(min), uMax(max) {
}

double PIDController::calculate(double setpoint, double output, double dt) {
    if (dt <= 0) {
        return control;
    }

    double error = setpoint - output;

    double deltaControl =
        kp * (error - previousError)
        + ki * error * dt
        + kd / dt * (error - 2.0 * previousError
        + previousPreviousError);

    control += deltaControl;

    if (control > uMax) {
        control = uMax;
    }
    else if (control < uMin) {
        control = uMin;
    }

    previousPreviousError = previousError;
    previousError = error;

    return control;
}

void PIDController::reset() {
    previousError = 0.0;
    previousPreviousError = 0.0;
    control = 0.0;
}