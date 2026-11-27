#pragma once

enum class InputKind
{
    Step = 1,
    Impulse = 2,
    Harmonic = 3
};

double sampleSignal(InputKind type, int tau, double amplitude);
