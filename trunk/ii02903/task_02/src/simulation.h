#pragma once
#include <vector>

#include "models.h"
#include "signals.h"

// One row of the result table.
struct Row {
    int tau;   // step number
    double t;  // time, t = tau * dt
    double u;  // input
    double y;  // output
};

// Runs the model for n steps with the given input signal.
// Returns n + 1 rows (tau = 0..n): the row with number tau holds y(tau) and u(tau).
// Throws std::invalid_argument if n < 0.
std::vector<Row> simulate(Model& model, const InputSignal& input, int n);
