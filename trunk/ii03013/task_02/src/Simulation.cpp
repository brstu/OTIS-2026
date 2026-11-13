#include "Simulation.h"

#include <stdexcept>

std::vector<double> buildInputSignal(
    InputSignalType type,
    int steps,
    double dt,
    double amplitude,
    double omega)
{
    if (steps < 0)
    {
        throw std::invalid_argument("Number of steps must not be negative.");
    }

    std::vector<double> u(static_cast<std::size_t>(steps) + 1);

    for (int t = 0; t <= steps; ++t)
    {
        u[static_cast<std::size_t>(t)] = generateInput(
            type,
            static_cast<std::size_t>(t),
            t * dt,
            amplitude,
            omega
        );
    }

    return u;
}

std::vector<double> simulate(
    const Model& model,
    const std::vector<double>& u,
    double initialY,
    double dt)
{
    std::vector<double> y(u.size(), 0.0);

    if (u.empty())
    {
        return y;
    }

    y[0] = initialY;

    for (std::size_t t = 0; t + 1 < u.size(); ++t)
    {
        const double delayedU = (t >= 2) ? u[t - 2] : 0.0;

        y[t + 1] = model.next(
            y[t],
            u[t],
            delayedU,
            dt
        );
    }

    return y;
}
