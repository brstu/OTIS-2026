#pragma once

#include <vector>

#include "InputSignals.h"
#include "models.h"

// Формирует входной сигнал из steps+1 отсчётов (t = 0..steps, время t*dt).
// Бросает std::invalid_argument при steps < 0.
std::vector<double> buildInputSignal(
    InputSignalType type,
    int steps,
    double dt,
    double amplitude,
    double omega
);

// Моделирование одной модели: y[0] = initialY,
// y[t+1] = model.next(y[t], u[t], u[t-2] (0 при t < 2), dt).
std::vector<double> simulate(
    const Model& model,
    const std::vector<double>& u,
    double initialY,
    double dt
);
