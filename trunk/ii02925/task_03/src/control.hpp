#pragma once

#include <string>
#include <vector>

/** One row of a closed-loop simulation result. */
struct Sample {
    int step;
    double setpoint;
    double output;
    double error;
    double control;
};

/** Coefficients of linear plant Model 1.9. */
struct LinearPlantParameters {
    double a1, a2, a3, b1;
};

/** Coefficients of nonlinear plant Model 2.1. */
struct NonlinearPlantParameters {
    double a, b, c, d;
};

/** Coefficients and integration step of differential plant Model 3.3. */
struct DifferentialPlantParameters {
    double a, b, dt;
};

/** Base interface for a controlled object (plant). */
class Plant {
public:
    virtual ~Plant() = default;
    /** Applies control u and returns the new plant output. */
    virtual double update(double u) = 0;
    /** Restores zero initial conditions. */
    virtual void reset() = 0;
    /** Returns the current output without changing the state. */
    virtual double output() const = 0;
    /** Short name used in result file names. */
    virtual std::string name() const = 0;
};

/** Variant 9 linear plant: model 1.9. */
class LinearPlant19 final : public Plant {
    LinearPlantParameters p_;
    double y0_ = 0.0, y1_ = 0.0, y2_ = 0.0;
public:
    explicit LinearPlant19(LinearPlantParameters parameters = {0.5, 0.2, 0.1, 0.4});
    double update(double u) override;
    void reset() override;
    double output() const override;
    std::string name() const override;
};

/** Variant 9 nonlinear plant: model 2.1. */
class NonlinearPlant21 final : public Plant {
    NonlinearPlantParameters p_;
    double y0_ = 0.0, y1_ = 0.0, previousU_ = 0.0;
public:
    explicit NonlinearPlant21(NonlinearPlantParameters parameters = {0.5, 0.05, 0.4, 0.1});
    double update(double u) override;
    void reset() override;
    double output() const override;
    std::string name() const override;
};

/** Variant 9 differential plant 3.3 integrated by Euler's method. */
class DifferentialPlant33 final : public Plant {
    DifferentialPlantParameters p_;
    double y_ = 0.0;
public:
    explicit DifferentialPlant33(DifferentialPlantParameters parameters = {0.6, 1.0, 0.1});
    double update(double u) override;
    void reset() override;
    double output() const override;
    std::string name() const override;
};

/** Simple positional discrete PID controller with output limiting and anti-windup. */
class PIDController {
public:
    PIDController(double kp, double ki, double kd, double dt,
                  double minimumControl, double maximumControl);
    /** Calculates a new control value from the current error. */
    double update(double error);
    /** Clears integral and previous-error state. */
    void reset();
    double kp() const { return kp_; }
    double ki() const { return ki_; }
    double kd() const { return kd_; }
private:
    double kp_, ki_, kd_, dt_, min_, max_;
    double integral_ = 0.0, previousError_ = 0.0;
    bool first_ = true;
};

/** Runs a fixed number of closed-loop simulation steps. */
std::vector<Sample> runFixed(Plant& plant, PIDController& pid,
                             double setpoint, int steps);

/** Runs until error stays inside epsilon for stableSteps consecutive steps. */
std::vector<Sample> runUntilSettled(Plant& plant, PIDController& pid,
                                    double setpoint, double epsilon,
                                    int maximumSteps, int stableSteps = 5);

/** Writes strict CSV table and a dependency-free SVG graph. */
void saveResults(const std::string& baseName, const std::vector<Sample>& samples,
                 const std::string& graphTitle = "",
                 double scaleMinimum = 0.0, double scaleMaximum = 0.0);
