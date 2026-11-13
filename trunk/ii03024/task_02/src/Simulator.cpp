#include "Simulator.h"

std::vector<Sample> simulate(Model& model, const ISignal& signal, int n, double dt) {
    std::vector<Sample> result;
    if (n < 0) return result;
    result.reserve(static_cast<size_t>(n) + 1);

    for (int tau = 0; tau <= n; ++tau) {
        const double u = signal.value(tau);
        const double y = model.nextStep(u, dt);
        result.push_back({tau, u, y});
    }
    return result;
}
