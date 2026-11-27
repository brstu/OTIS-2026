#include "Simulation.h"

#include <utility>

SignalInputSource::SignalInputSource(InputKind type, double amplitude)
    : type_(type), amplitude_(amplitude) {}

double SignalInputSource::valueAt(int tau) const
{
    return sampleSignal(type_, tau, amplitude_);
}

InputKind SignalInputSource::getType() const { return type_; }
double SignalInputSource::getAmplitude() const { return amplitude_; }

CallableInputSource::CallableInputSource(std::function<double(int)> function)
    : function_(std::move(function)) {}

double CallableInputSource::valueAt(int tau) const
{
    return function_ ? function_(tau) : 0.0;
}

std::vector<SimStep> simulate(const Model& model,
                              const SimulationConfig& config,
                              const IInputSource& input)
{
    const int steps = config.steps < 0 ? 0 : config.steps;

    std::vector<SimStep> rows;
    rows.reserve(static_cast<std::size_t>(steps) + 1);

    double y = config.y0;
    double yPrev = config.y0;
    double yPrev2 = config.y0;

    rows.push_back({0, input.valueAt(0), y});

    for (int tau = 0; tau < steps; ++tau)
    {
        const double u = input.valueAt(tau);
        const double nextY = model.computeNext(y, yPrev, yPrev2, u, config.dt);

        yPrev2 = yPrev;
        yPrev = y;
        y = nextY;

        rows.push_back({tau + 1, input.valueAt(tau + 1), y});
    }

    return rows;
}
