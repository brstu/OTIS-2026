#include "SignalSource.h"
#include <cmath>

double sampleSignal(InputKind type, int tau, double amplitude)
{
    switch (type)
    {
    case InputKind::Step:

        return amplitude;

    case InputKind::Impulse:

        return (tau == 0) ? amplitude : 0.0;

    case InputKind::Harmonic:

        return amplitude * std::sin(static_cast<double>(tau));
    }

    return 0.0;
}
