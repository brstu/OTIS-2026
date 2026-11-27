#pragma once
#include "InputSignals.h"
#include "Model.h"
#include <iosfwd>
#include <stdexcept>

class SimulationError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void simulate(Model& model, const InputSignal& signal, int steps,
              std::ostream& csv, std::ostream& console);
