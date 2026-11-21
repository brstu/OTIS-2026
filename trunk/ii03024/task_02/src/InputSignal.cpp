#include "InputSignal.h"

#include <cmath>

StepSignal::StepSignal(double amplitude) : amplitude_(amplitude) {}
double StepSignal::value(int) const { return amplitude_; }

PulseSignal::PulseSignal(double amplitude) : amplitude_(amplitude) {}
double PulseSignal::value(int tau) const { return tau == 0 ? amplitude_ : 0.0; }

HarmonicSignal::HarmonicSignal(double amplitude, double phaseStep)
    : amplitude_(amplitude), phaseStep_(phaseStep) {}
double HarmonicSignal::value(int tau) const { return amplitude_ * std::sin(tau * phaseStep_); }
