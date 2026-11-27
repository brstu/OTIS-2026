#pragma once

#include <functional>
#include <vector>

#include "SignalSource.h"
#include "Model.h"

struct SimStep
{
    int tau = 0;
    double u = 0.0;
    double y = 0.0;
};

struct SimulationConfig
{
    int steps = 10;
    double y0 = 0.0;
    double dt = 1.0;
};

class IInputSource
{
public:
    virtual ~IInputSource() = default;

    virtual double valueAt(int tau) const = 0;
};

class SignalInputSource final : public IInputSource
{
public:
    SignalInputSource(InputKind type, double amplitude);

    double valueAt(int tau) const override;

    InputKind getType() const;
    double getAmplitude() const;

private:
    InputKind type_;
    double amplitude_;
};

class CallableInputSource final : public IInputSource
{
public:
    explicit CallableInputSource(std::function<double(int)> function);

    double valueAt(int tau) const override;

private:
    std::function<double(int)> function_;
};

std::vector<SimStep> simulate(const Model& model,
                              const SimulationConfig& config,
                              const IInputSource& input);
