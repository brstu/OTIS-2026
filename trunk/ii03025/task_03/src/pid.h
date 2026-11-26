#pragma once

class PIDController {

private:

    double kp;
    double ki;
    double kd;

    double uMin;
    double uMax;

    double previousError = 0.0;
    double previousPreviousError = 0.0;

    double control = 0.0;

public:

    PIDController(double p, double i, double d,
                  double min, double max);

    double calculate(double setpoint, double output, double dt);
    void reset();

};
