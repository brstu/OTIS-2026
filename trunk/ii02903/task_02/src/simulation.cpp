#include "simulation.h"

#include <stdexcept>

std::vector<Row> simulate(Model& model, const InputSignal& input, int n) {
    if (n < 0) throw std::invalid_argument("number of steps must not be negative");

    const double dt = model.timeStep();
    std::vector<Row> rows;
    rows.reserve(static_cast<size_t>(n) + 1);
    for (int tau = 0; tau <= n; ++tau) {
        const double t = tau * dt;
        const double u = input.value(tau, t);
        rows.push_back({tau, t, u, model.current()});
        if (tau < n) model.nextStep(u);
    }
    return rows;
}
