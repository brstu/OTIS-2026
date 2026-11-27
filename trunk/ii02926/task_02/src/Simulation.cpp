#include "Simulation.h"

#include <cstddef>
#include <ostream>
#include <stdexcept>

std::vector<Step> simulate(Model& model, const InputSignal& signal, int n) {
    if (n < 1) {
        throw std::invalid_argument("n must be >= 1");
    }

    model.reset();

    std::vector<Step> steps;
    steps.reserve(static_cast<std::size_t>(n));
    for (int tau = 1; tau <= n; ++tau) {
        const double u = signal.value(tau);
        const double y = model.next(u);
        steps.push_back({tau, u, y});
    }
    return steps;
}

bool printStabilityWarning(const Model& model, std::ostream& out) {
    if (model.isStable()) {
        return false;
    }
    out << "\n!!! WARNING !!!\n" << model.stabilityWarning() << "\n";
    return true;
}
