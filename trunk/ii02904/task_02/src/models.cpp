#include "models.h"

#include <cmath>
#include <stdexcept>

namespace {
void requirePositiveTimeStep(double timeStep)
{
    if (!std::isfinite(timeStep) || timeStep <= 0.0) {
        throw std::invalid_argument("The Euler time step must be finite and positive.");
    }
}
}  // namespace

Model14::Model14(double a, double b, double initialOutput)
    : a_(a), b_(b), output_(initialOutput)
{
}

double Model14::nextStep(double input)
{
    output_ = a_ * output_ + b_ * (input - previousInput_);
    previousInput_ = input;
    return output_;
}

std::string Model14::name() const
{
    return "Модель 1.4";
}

Model26::Model26(double a1, double a2, double b, double initialOutput)
    : a1_(a1), a2_(a2), b_(b), output_(initialOutput)
{
}

double Model26::nextStep(double input)
{
    const double nextOutput = a1_ * output_ * output_ * output_
        - a2_ * previousOutput_ + b_ * input * input;
    previousOutput_ = output_;
    output_ = nextOutput;
    return output_;
}

std::string Model26::name() const
{
    return "Модель 2.6";
}

Model38::Model38(double a1, double a2, double b, double timeStep, double initialOutput)
    : a1_(a1), a2_(a2), b_(b), timeStep_(timeStep), output_(initialOutput)
{
    requirePositiveTimeStep(timeStep_);
}

double Model38::nextStep(double input)
{
    const double derivative = -a1_ * output_ - a2_ * output_ * output_ + b_ * input;
    output_ += timeStep_ * derivative;
    return output_;
}

std::string Model38::name() const
{
    return "Модель 3.8";
}
