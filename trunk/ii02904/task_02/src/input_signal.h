#pragma once

enum class InputSignal {
    Step,
    Impulse,
    Harmonic
};

double inputAt(InputSignal signal, int tau, double stepValue = 1.0);
